#include <unistd.h>
#include <cstdio>
#include "root_dir.hpp"
#include "dir_entry.hpp"

void printRootDir(int fd, uint32_t rootOffset, BSStruct *bootSector, unsigned char FAT[], int fat_type){
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
//            if (dirEntry.Name[0] == 'I' && dirEntry.Name[1] == 'O'){
                check_DirEntry(&dirEntry, bootSector, fd, FAT, fat_type);
//                break;
//            }
        }
    }
}