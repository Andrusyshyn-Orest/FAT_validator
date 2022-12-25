#ifndef CHECKING_FAT_FATS_H
#define CHECKING_FAT_FATS_H

#include <unistd.h>
#include <stdio.h>
#include "structs.h"

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
// Return an unsigned short containing the 12-bit FAT entry code at FATindex

unsigned short GetFatEntry(int FATindex, const unsigned char FAT[]);


// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void SetFatEntry(int FATindex, unsigned short FAT12ClusEntryVal, unsigned char FAT[]);


int compare_FATs(BSStruct *bootSector, int fd);

#endif //CHECKING_FAT_FATS_H
