#ifndef CHECKING_FAT_DIR_H
#define CHECKING_FAT_DIR_H

#include <type_traits>
#include "structs.hpp"
#include "dir_entry.hpp"
#include "trim.hpp"
#include <iostream>
#include <vector>
#include <cstring>


template<
        typename T ,
        typename = typename std::enable_if<
                std::is_base_of<BSStruct, T>::value ||
                std::is_base_of<BSStruct32 , T>::value
        >::type
>
void printDir(int fd, uint32_t offset, T *bootSector, unsigned char FAT[], int fat_type, const char* name){
    // printf("----ROOT DIRECTORY----\n");
    printf("\nDirectory name: %s\n", name);
    lseek(fd, offset, SEEK_SET);
    // printf("\n%x", offset);
    int i = 0;
    std::vector<DirEntry> directories;
    while (1){
        lseek(fd, offset + i * 32, SEEK_SET);
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
            check_DirEntry(&dirEntry, bootSector, fd, FAT, fat_type);
        } else {
            if ((dirEntry.Name[0] != '.' && dirEntry.Name[1] != ' ') || (dirEntry.Name[0] != '.' && dirEntry.Name[1] != '.' && dirEntry.Name[2] != ' ')) {
                directories.push_back(dirEntry);
            }
        }
    }

    for (DirEntry dirent : directories) {
        int offset;
        int first_data_sector;
        if ( (fat_type &FAT12) || (fat_type &FAT16)) {
            int root_dir_sectors = ((bootSector->BPB_RootEntCnt * 32) + (bootSector->BPB_BytsPerSec - 1)) / bootSector->BPB_BytsPerSec;
            first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors;
        } else if (fat_type &FAT32) {
            first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz32);
        }

        offset = (first_data_sector + (dirent.startCluster - 2) * bootSector->BPB_SecPerClus) * bootSector->BPB_BytsPerSec;


        int j = 7;
        char name_string[13] = "            ";
        strncpy(name_string, (char*)dirent.Name, 8);
        while (name_string[j] == ' ') j--;
        name_string[j+1] = '.';                         // Add extension
        strncpy(&name_string[j+2], (char*)dirent.Extension, 3);
        while (name_string[j+4] == ' ') j--;
        if (name_string[j+4] == '.') name_string[j+4] = ' ';
        std::string new_name = name;
        new_name += trim(name_string);
        new_name += "/";

        printDir(fd, offset, bootSector, FAT, fat_type, new_name.c_str());

    }
}
#endif //CHECKING_FAT_DIR_H
