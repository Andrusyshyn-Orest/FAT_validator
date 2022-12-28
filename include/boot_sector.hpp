#ifndef CHECKING_FAT_BOOT_SECTOR_H
#define CHECKING_FAT_BOOT_SECTOR_H

#include <type_traits>
#include "structs.hpp"
#include "fats_versions.hpp"

template<
        typename T ,
        typename = typename std::enable_if<
                std::is_base_of<BSStruct, T>::value ||
                std::is_base_of<BSStruct32 , T>::value
        >::type
>
void printBootSector(T* bootStruct, int fat_type) {
    printf("----BOOT SECTOR----\n\n");
    printf("Jump instruction: 0x");
    for (int i = 0; i < 3; ++i) {
        printf("%x", bootStruct->BS_jmpBoot[i]);
    }
    printf("\n");

    printf("OEM Name in text: ");
    for (int i = 0; i < 8; ++i) {
        printf("%c", bootStruct->BS_OEMName[i]);
    }
    printf("\n");

    printf("\n");
    printf("BIOS Parameter Block: \n");
    printf("Bytes per Sector: %hu\n", bootStruct->BPB_BytsPerSec);
    printf("Sectors Per Cluster: %i\n", bootStruct->BPB_SecPerClus);
    printf("Reserved Sectors: %hu\n", bootStruct->BPB_RsvdSecCnt);
    printf("Number of file allocation tables (FATs): %i\n", bootStruct->BPB_NumFATs);
    printf("Root Entries: %hu\n", bootStruct->BPB_RootEntCnt);
    printf("Small Sectors: %hu\n", bootStruct->BPB_TotSec16);
    printf("Media Type: 0x%x\n", bootStruct->BPB_Media);
    printf("Sectors per file allocation table (FAT): %hu\n", bootStruct->BPB_FATSz16);
    printf("Sectors per Track: %hu\n", bootStruct->BPB_SecPerTrk);
    printf("Heads: %hu\n", bootStruct->BPB_NumHeads);
    printf("Hidden Sectors: %u\n", bootStruct->BPB_HiddSec);
    printf("Large Sectors: %u\n", bootStruct->BPB_TotSec32);

    if (fat_type &FAT32) {
        printf("Sector size of FAT32 table: %u\n", bootStruct->BPB_FATSz32);
        printf("Flags: %hu\n", bootStruct->BPB_Flags);
        printf("FAT version number: %hu\n", bootStruct->BPB_FATversion);
        printf("The cluster number of the root directory: %u\n", bootStruct->BPB_RootClustNum);
        printf("The sector number of the FSInfo structure: %hu\n", bootStruct->BPB_FSInfoSecNum);
        printf("The sector number of the backup boot sector: %hu\n", bootStruct->BPB_BupBSSecNum);
        printf("Reserved: 0x");
        for (int i = 0; i < 12; ++i) {
            printf("%x", bootStruct->BPB_Reserved[i]);
        }
        printf("\n");
    }

    printf("Physical Disk Number: %i\n", bootStruct->BS_DrvNum);
    printf("Reserved (used by Windows NT): 0x%x\n", bootStruct->BS_Reserved1);
    printf("Extended boot signature: 0x%x\n", bootStruct->BS_BootSig);
    printf("Volume serial number: %u\n", bootStruct->BS_VolID);
    printf("Volume label: ");
    for (int i = 0; i < 11; ++i) {
        printf("%c", bootStruct->BS_VolLab[i]);
    }
    printf("\n");
    printf("Deceptive FAT type Label: ");
    for (int i = 0; i < 8; ++i) {
        printf("%c", bootStruct->BS_FilSysType[i]);
    }
    printf("\n");

}

#endif //CHECKING_FAT_BOOT_SECTOR_H
