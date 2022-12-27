#include "fats.hpp"
#include "fats_versions.hpp"


uint32_t GetFatEntry(int FATindex, const unsigned char FAT[], int fat_type)
{
    if (fat_type & FAT12) {
        return GetFatEntry12(FATindex, FAT);
    } else if (fat_type &FAT16) {
        return GetFatEntry16(FATindex, FAT);
    } else if (fat_type &FAT32) {
        return GetFatEntry32(FATindex, FAT);
    } else {
        return -1;
    }
}

void SetFatEntry(int FATindex, uint32_t FATClusEntryVal, unsigned char FAT[], int fat_type)
{
    if (fat_type & FAT12) {
        SetFatEntry12(FATindex, FATClusEntryVal, FAT);
    } else if (fat_type & FAT16) {
        SetFatEntry16(FATindex, FATClusEntryVal, FAT);
    } else if (fat_type & FAT32) {
        SetFatEntry32(FATindex, FATClusEntryVal, FAT);
    } else {
        printf("Error in SetFatEntry: wrong fat type.");
    }
}

unsigned short GetFatEntry12(int FATindex, const unsigned char FAT[])
{

    unsigned short FATEntryCode;           // The return value
    int FatOffset = ((FATindex * 3) / 2);  // Calculate the offset

    if (FATindex % 2 == 1)                 // If the index is odd
    {  // Pull out a unsigned short from a unsigned char array

        FATEntryCode = *((unsigned short *) &FAT[FatOffset]);
        FATEntryCode >>= 4;                 // Extract the high-order 12 bits

    }
    else                                   // If the index is even
    {  // Pull out a unsigned short from a unsigned char array

        FATEntryCode = *((unsigned short *) &FAT[FatOffset]);

        FATEntryCode &= 0x0fff;             // Extract the low-order 12 bits

    }
    return FATEntryCode;

} // End GetFatEntry


// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void SetFatEntry12(int FATindex, unsigned short FAT12ClusEntryVal, unsigned char FAT[])
{

    int FATOffset = ((FATindex * 3) / 2);  // Calculate the offset
    int FATData = *((unsigned short*)&FAT[FATOffset]);

    if (FATindex % 2 == 0)                 // If the index is even
    {  FAT12ClusEntryVal &= 0x0FFF;        // mask to 12 bits

        FATData &= 0xF000;                  // mask complement

    }
    else                                   // Index is odd
    {  FAT12ClusEntryVal <<= 4;            // move 12-bits high

        FATData &= 0x000F;                  // mask complement

    }

// Update FAT entry value in the FAT table
    *((unsigned short *)&FAT[FATOffset]) = FATData | FAT12ClusEntryVal;

} // End SetFatEntry

unsigned short GetFatEntry16(int FATindex, const unsigned char FAT[]) {
    unsigned short FATEntryCode;
    int FatOffset = FATindex * 2;
    FATEntryCode = *((unsigned short *) &FAT[FatOffset]);
    return FATEntryCode;
}

void SetFatEntry16(int FATindex, unsigned short FAT16ClusEntryVal, unsigned char FAT[]) {
    int FatOffset = FATindex * 2;
    *((unsigned short *)&FAT[FatOffset]) = FAT16ClusEntryVal;
}

uint32_t GetFatEntry32(int FATindex, const unsigned char FAT[]) {
    uint32_t FATEntryCode;
    int FatOffset = FATindex * 4;
    FATEntryCode = *((uint32_t *) &FAT[FatOffset]);
    FATEntryCode &= 0x0FFFFFFF;
    return FATEntryCode;
}

void SetFatEntry32(int FATindex, uint32_t FAT32ClusEntryVal, unsigned char FAT[]) {
    uint32_t FATEntryCode;
    int FatOffset = FATindex * 4;
    FATEntryCode = *((uint32_t *) &FAT[FatOffset]);
    FATEntryCode &= 0xF0000000;
    FAT32ClusEntryVal &= 0x0FFFFFFF;
    *((uint32_t *)&FAT[FatOffset]) = FATEntryCode | FAT32ClusEntryVal;
}


int compare_FATs(BSStruct *bootSector, int fd) {
    unsigned char FAT1[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec];
    lseek(fd, (bootSector->BPB_RsvdSecCnt)  * bootSector->BPB_BytsPerSec, SEEK_SET);
    read(fd, &FAT1, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

    unsigned char tmp_FAT[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec];
    for (int i = 0; i < bootSector->BPB_NumFATs - 1; ++i) {
        read(fd, &tmp_FAT, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

        for (int j = 0; j < bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec; ++j) {
            if (FAT1[j] != tmp_FAT[j]) {
                printf("\nincompatibility in %i FAT table on %i byte!\n", i+1, j);
                return 1;
            }
        }
    }

    return 0;
}