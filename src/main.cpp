#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "structs.hpp"
#include "fats.hpp"
#include "boot_sector.hpp"
#include "fats_versions.hpp"
#include "print_dir.hpp"

size_t calc_fat12_byte(size_t cluster_ind) {
    return ((32 + 2*1261) + (cluster_ind - 2)) * 512;
}

int main(int argc, char *argv[]) {
    int fd;
    int fat_type;

    fd = open(argv[1], O_RDONLY);

    BSStruct32 check_fat_struct;
    read(fd, &check_fat_struct, sizeof(check_fat_struct));
    if (check_fat_struct.BPB_TotSec32 == 0) {
        int root_dir_sectors = ((check_fat_struct.BPB_RootEntCnt * 32) + (check_fat_struct.BPB_BytsPerSec - 1)) / check_fat_struct.BPB_BytsPerSec;
        int data_sectors = check_fat_struct.BPB_TotSec16 - (check_fat_struct.BPB_RsvdSecCnt + (check_fat_struct.BPB_NumFATs * check_fat_struct.BPB_FATSz16) + root_dir_sectors);
        int total_clusters = data_sectors / check_fat_struct.BPB_SecPerClus;
        if (total_clusters < 4085) {
            printf("FAT12\n");
            fat_type = FAT12;
        } else {
            printf("FAT16\n");
            fat_type = FAT16;
        }
    } else if (check_fat_struct.BPB_TotSec16 == 0) {
        fat_type = FAT32;
        printf("FAT32\n");
    } else {
        std::cerr << "Unsupported FAT type!" << std::endl;
        exit(-1);
    }

    lseek(fd, 0, SEEK_SET);
    if (fat_type &FAT16 || fat_type &FAT12) {
        BSStruct bootSector;       // The BYTE-aligned structure.
        read(fd, &bootSector, sizeof(bootSector));
        printBootSector(&bootSector, fat_type);

        printf("\n");


        if (compare_FATs(&bootSector, fd, fat_type) == 1) {
            printf("\nDifferent FAT tables!\n");
            return -1;
        }
        unsigned char FAT[bootSector.BPB_FATSz16 * bootSector.BPB_BytsPerSec];
        lseek(fd, (bootSector.BPB_RsvdSecCnt)  * bootSector.BPB_BytsPerSec, SEEK_SET);
        read(fd, &FAT, bootSector.BPB_FATSz16 * bootSector.BPB_BytsPerSec);

        uint32_t offset = (bootSector.BPB_RsvdSecCnt + bootSector.BPB_NumFATs*bootSector.BPB_FATSz16) * bootSector.BPB_BytsPerSec;
        SetFatEntry(23, 21, FAT, fat_type);
        printDir(fd, offset, &bootSector, FAT, fat_type, "/");

        close(fd);

    } else {
        BSStruct32 bootSector;       // The BYTE-aligned structure.
        read(fd, &bootSector, sizeof(bootSector));
        printBootSector(&bootSector, fat_type);

        printf("\n");

        if (compare_FATs(&bootSector, fd, fat_type) == 1) {
            printf("\nDifferent FAT tables!\n");
            return -1;
        }

        unsigned char FAT[bootSector.BPB_FATSz32 * bootSector.BPB_BytsPerSec];
        lseek(fd, (bootSector.BPB_RsvdSecCnt)  * bootSector.BPB_BytsPerSec, SEEK_SET);
        read(fd, &FAT, bootSector.BPB_FATSz32 * bootSector.BPB_BytsPerSec);

        uint32_t offset = (bootSector.BPB_RsvdSecCnt + bootSector.BPB_NumFATs*bootSector.BPB_FATSz32) * bootSector.BPB_BytsPerSec
                + (bootSector.BPB_RootClustNum - 2) * bootSector.BPB_SecPerClus * bootSector.BPB_BytsPerSec;
//    SetFatEntry(23, 21, FAT, fat_type);
        printDir(fd, offset, &bootSector, FAT, fat_type, "/");

        close(fd);

    }

    return 0;
}
