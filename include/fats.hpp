#ifndef CHECKING_FAT_FATS_H
#define CHECKING_FAT_FATS_H

#include <unistd.h>
#include <stdio.h>
#include "structs.hpp"


uint32_t GetFatEntry(int FATindex, const unsigned char FAT[], int fat_type);

void SetFatEntry(int FATindex, uint32_t FATClusEntryVal, unsigned char FAT[], int fat_type);

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
// Return an unsigned short containing the 12-bit FAT entry code at FATindex
unsigned short GetFatEntry12(int FATindex, const unsigned char FAT[]);

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void SetFatEntry12(int FATindex, unsigned short FAT12ClusEntryVal, unsigned char FAT[]);

unsigned short GetFatEntry16(int FATindex, const unsigned char FAT[]);

void SetFatEntry16(int FATindex, unsigned short FAT16ClusEntryVal, unsigned char FAT[]);

uint32_t GetFatEntry32(int FATindex, const unsigned char FAT[]);

void SetFatEntry32(int FATindex, uint32_t FAT32ClusEntryVal, unsigned char FAT[]);

int compare_FATs(BSStruct *bootSector, int fd);

#endif //CHECKING_FAT_FATS_H
