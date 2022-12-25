#include <stdio.h>
#include <unistd.h>
#include "root_dir.h"
#include "dir_entry.h"

void printRootDir(int fd, uint32_t rootOffset, BSStruct *bootSector, unsigned char FAT[]){
    printf("----ROOT DIRECTORY----\n");
    lseek(fd, rootOffset, SEEK_SET);
    int i = 0;
    while (1){
        lseek(fd, rootOffset + i * 32, SEEK_SET);
        i++;
        DirEntry dirEntry;
        read(fd, &dirEntry, sizeof(dirEntry));
        // No dirEntries after
        if (dirEntry.Name[0] == 0x00) {
            break;
        }

        // DirEntry is long entry
        if (dirEntry.Name[11] == 0x0F) {
            continue;
        }

        // DirEntry is unused
        if (dirEntry.Name[0] == 0xE5) {
            continue;
        }

        PrintDirectoryEntry(&dirEntry);
        if (!(dirEntry.Attributes &DIRECTORY)) {
            check_DirEntry(&dirEntry, bootSector, fd, FAT);
        }
    }
}