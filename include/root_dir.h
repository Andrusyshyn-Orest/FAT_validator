#include "structs.h"

#ifndef CHECKING_FAT_ROOT_DIR_H
#define CHECKING_FAT_ROOT_DIR_H

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void printRootDir(int fd, uint32_t rootOffset, BSStruct *bootSector, unsigned char FAT[]);
#endif //CHECKING_FAT_ROOT_DIR_H
