#include <fcntl.h>
#include <unistd.h>
#include <unordered_set>
#include <iostream>
#include "structs.hpp"
#include "fats.hpp"
#include "boot_sector.hpp"
#include "root_dir.hpp"
#include "fats_versions.hpp"

size_t calc_fat12_byte(size_t cluster_ind) {
    return ((32 + 2*1261) + (cluster_ind - 2)) * 512;
}

int main() {
    int fd;

//    fd = open("../fat12_fs.img", O_RDONLY);
//    int fat_type = FAT12;
//    fd = open("../hd0_just_FAT16_without_MBR.img", O_RDONLY);
//    int fat_type = FAT16;
    fd = open("../fat32_fs_without_mbr_2.img", O_RDONLY);
    int fat_type = FAT32;

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
//    SetFatEntry(23, 21, FAT, fat_type);
//    printRootDir(fd, offset, &bootSector, FAT, fat_type);


//    printf("\n%i\n", GetFatEntry(28, FAT, fat_type));

        int start_cluster = 43;
//    while (1) {
//        if ((start_cluster <= 0xFFFF) && (start_cluster >= 0xFFF8)) {
//            break;
//        }
//        printf("\n%i -> %i\n", start_cluster, GetFatEntry(start_cluster, FAT, fat_type));
//        start_cluster = GetFatEntry(start_cluster, FAT, fat_type);
//    }

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
        printRootDir(fd, offset, &bootSector, FAT, fat_type);


//    printf("\n%i\n", GetFatEntry(28, FAT, fat_type));

        int start_cluster = 2;
        while (1) {
            if ((start_cluster <= 0xFFFFFFFF) && (start_cluster >= 0xFFFFFF8)) {
                break;
            }
            printf("\n%i -> %i\n", start_cluster, GetFatEntry(start_cluster, FAT, fat_type));
            start_cluster = GetFatEntry(start_cluster, FAT, fat_type);
        }


        close(fd);

        printf("\n0x%x", calc_fat12_byte(32261));
    }

    return 0;
}
