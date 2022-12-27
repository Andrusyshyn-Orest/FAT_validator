#include "structs.hpp"

#ifndef CHECKING_FAT_ROOT_DIR_H
#define CHECKING_FAT_ROOT_DIR_H

void printRootDir(int fd, uint32_t rootOffset, BSStruct *bootSector, unsigned char FAT[], int fat_type);
#endif //CHECKING_FAT_ROOT_DIR_H
