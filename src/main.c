#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint-gcc.h>
#include "structs.h"
#include "fats.h"
#include "boot_sector.h"
#include "root_dir.h"

int main() {
    int fd;
    fd = open("../fat12_fs.img", O_RDONLY);

    BSStruct bootSector;       // The BYTE-aligned structure.
    read(fd, &bootSector, sizeof(bootSector));
    printBootSector(&bootSector);

    uint32_t offset = (bootSector.BPB_RsvdSecCnt + bootSector.BPB_NumFATs*bootSector.BPB_FATSz16) * bootSector.BPB_BytsPerSec;
    printf("\n");


    if (compare_FATs(&bootSector, fd) == 1) {
        printf("\nDifferent FAT tables!\n");
        return -1;
    }
    unsigned char FAT[bootSector.BPB_FATSz16 * bootSector.BPB_BytsPerSec];
    lseek(fd, (bootSector.BPB_RsvdSecCnt)  * bootSector.BPB_BytsPerSec, SEEK_SET);
    read(fd, &FAT, bootSector.BPB_FATSz16 * bootSector.BPB_BytsPerSec);

    SetFatEntry(23, 21, FAT);
    printRootDir(fd, offset, &bootSector, FAT);

//    printf("\n%i\n", GetFatEntry(23, FAT));
    close(fd);

    return 0;
}
