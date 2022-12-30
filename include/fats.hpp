#ifndef CHECKING_FAT_FATS_H
#define CHECKING_FAT_FATS_H

#include <type_traits>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include "structs.hpp"
#include "fats_versions.hpp"


uint32_t GetFatEntry(uint32_t FATindex, const unsigned char FAT[], int fat_type);

void SetFatEntry(uint32_t FATindex, uint32_t FATClusEntryVal, unsigned char FAT[], int fat_type);

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
// Return an unsigned short containing the 12-bit FAT entry code at FATindex
unsigned short GetFatEntry12(int FATindex, const unsigned char FAT[]);

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void SetFatEntry12(int FATindex, unsigned short FAT12ClusEntryVal, unsigned char FAT[]);

unsigned short GetFatEntry16(int FATindex, const unsigned char FAT[]);

void SetFatEntry16(int FATindex, unsigned short FAT16ClusEntryVal, unsigned char FAT[]);

uint32_t GetFatEntry32(uint32_t FATindex, const unsigned char FAT[]);

void SetFatEntry32(uint32_t FATindex, uint32_t FAT32ClusEntryVal, unsigned char FAT[]);

template<
        typename T ,
        typename = typename std::enable_if<
                std::is_base_of<BSStruct, T>::value ||
                std::is_base_of<BSStruct32 , T>::value
        >::type
>
int compare_FATs(T *bootSector, int fd, int fat_type) {
    size_t FAT_size;
    if (fat_type &FAT12 || fat_type &FAT16) {
        FAT_size = bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec;
    } else if (fat_type &FAT32) {
        FAT_size = bootSector->BPB_FATSz32 * bootSector->BPB_BytsPerSec;
    } else {
        std::cerr << "Unsupported FAT type!" << std::endl;
        exit(-1);
    }

    unsigned char FAT1[FAT_size];
    lseek(fd, (bootSector->BPB_RsvdSecCnt)  * bootSector->BPB_BytsPerSec, SEEK_SET);
    read(fd, &FAT1, FAT_size);

    unsigned char tmp_FAT[FAT_size];
    for (int i = 0; i < bootSector->BPB_NumFATs - 1; ++i) {
        read(fd, &tmp_FAT, FAT_size);

        for (int j = 0; j < FAT_size; ++j) {
            if (FAT1[j] != tmp_FAT[j]) {
                printf("\nincompatibility in %i FAT table on %i byte!\n", i+1, j);
                return 1;
            }
        }
    }

    return 0;
}

#endif //CHECKING_FAT_FATS_H
