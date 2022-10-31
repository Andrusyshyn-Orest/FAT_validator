#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint-gcc.h>

#define N_FILE       0x00
#define READ_ONLY    0x01
#define HIDDEN       0x02
#define SYSTEM       0x04
#define VOLUME       0x08 // this is the volume label entry
#define DIRECTORY    0x10
#define ARCHIVE      0x20 // same as file


// this struct was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
#pragma pack(push, 1)               /* Byte align in memory (no padding) */
typedef struct

{  unsigned char  BS_jmpBoot[3];    /* Jump instruction to the boot code */
    unsigned char  BS_OEMName[8];    /* Name of system that formatted the volume */
    unsigned short BPB_BytsPerSec;   /* Bytes per sector (should be 512) */
    unsigned char  BPB_SecPerClus;   /* Sectors per cluster (FAT-12 = 1) */
    unsigned short BPB_RsvdSecCnt;   /* Reserved sectors (FAT-12 = 1) */
    unsigned char  BPB_NumFATs;      /* FAT tables on the disk (should be 2) */
    unsigned short BPB_RootEntCnt;   /* Max directory entries in root directory */
    unsigned short BPB_TotSec16;     /* FAT-12 total number of sectors on the disk */
    unsigned char  BPB_Media;        /* Media type {fixed, removable, etc.} */
    unsigned short BPB_FATSz16;      /* Sector size of FAT table (FAT-12 = 9) */
    unsigned short BPB_SecPerTrk;    /* # of sectors per cylindrical track */
    unsigned short BPB_NumHeads;     /* # of heads per volume (1.4Mb 3.5" = 2) */
    uint32_t       BPB_HiddSec;      /* # of preceding hidden sectors (0) */
    uint32_t       BPB_TotSec32;     /* # of FAT-32 sectors (0 for FAT-12) */
    unsigned char  BS_DrvNum;        /* A drive number for the media (OS specific) */
    unsigned char  BS_Reserved1;     /* Reserved space for Windows NT (set to 0) */
    unsigned char  BS_BootSig;       /* (0x29) Indicates following: */
    uint32_t       BS_VolID;         /* Volume serial # (for tracking this disk) */
    unsigned char  BS_VolLab[11];    /* Volume label (RDL or "NO NAME    ") */
    unsigned char  BS_FilSysType[8]; /* Deceptive FAT type Label */

} BSStruct;
#pragma pack(pop)                   /* End strict alignment */


// this struct was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
#pragma pack(push, 1)            /* Byte align in memory (no padding) */
typedef struct

{  unsigned char  Name[8];       /* File name (capital letters, padded w/spaces) */
    unsigned char  Extension[3];  /* Extension (same format as name, no '.') */
    unsigned char  Attributes;    /* Holds the attributes code */
    unsigned char  Reserved[10];  /* Reserved for Windows NT (Set to zero!) */
    unsigned short Time;          /* Time of last write */
    unsigned short Date;          /* Date of last write */
    unsigned short startCluster;  /* Pointer to the first cluster of the file */
    uint32_t fileSize;      /* File size in bytes */
}  DirEntry;
#pragma pack(pop)                /* End strict alignment */


// this struct was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
#pragma pack(push,1)                // BYTE align in memory (no padding)
typedef struct
{                                   // (total 16 bits--a unsigned short)
    unsigned short sec: 5;           // low-order 5 bits are the seconds
    unsigned short min: 6;           // next 6 bits are the minutes
    unsigned short hour: 5;          // high-order 5 bits are the hour
} FATTime;
#pragma pack(pop)                   // End of strict alignment


// this struct was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
#pragma pack(push,1)                // BYTE align in memory (no padding)
typedef struct
{                                   // (total 16 bits--a unsigned short)
    unsigned short day: 5;           // low-order 5 bits are the day
    unsigned short month: 4;         // next 4 bits are the month
    unsigned short year: 7;          // high-order 7 bits are the year
} FATDate;
#pragma pack(pop)                   // End of strict alignment


void printBootSector(BSStruct bootStruct) {
    printf("----BOOT SECTOR----\n\n");
    printf("Jump instruction: 0x");
    for (int i = 0; i < 3; ++i) {
        printf("%x", bootStruct.BS_jmpBoot[i]);
    }
    printf("\n");

    printf("OEM Name in text: ");
    for (int i = 0; i < 8; ++i) {
        printf("%c", bootStruct.BS_OEMName[i]);
    }
    printf("\n");

    printf("\n");
    printf("BIOS Parameter Block: \n");
    printf("Bytes per Sector: %hu\n", bootStruct.BPB_BytsPerSec);
    printf("Sectors Per Cluster: %i\n", bootStruct.BPB_SecPerClus);
    printf("Reserved Sectors: %hu\n", bootStruct.BPB_RsvdSecCnt);
    printf("Number of file allocation tables (FATs): %i\n", bootStruct.BPB_NumFATs);
    printf("Root Entries: %hu\n", bootStruct.BPB_RootEntCnt);
    printf("Small Sectors: %hu\n", bootStruct.BPB_TotSec16);
    printf("Media Type: 0x%x\n", bootStruct.BPB_Media);
    printf("Sectors per file allocation table (FAT): %hu\n", bootStruct.BPB_FATSz16);
    printf("Sectors per Track: %hu\n", bootStruct.BPB_SecPerTrk);
    printf("Heads: %hu\n", bootStruct.BPB_NumHeads);
    printf("Hidden Sectors: %u\n", bootStruct.BPB_HiddSec);
    printf("Large Sectors: %u\n", bootStruct.BPB_TotSec32);
    printf("Physical Disk Number: %i\n", bootStruct.BS_DrvNum);
    printf("Reserved (used by Windows NT): 0x%x\n", bootStruct.BS_Reserved1);
    printf("Extended boot signature: 0x%x\n", bootStruct.BS_BootSig);
    printf("Volume serial number: %u\n", bootStruct.BS_VolID);
    printf("Volume label: ");
    for (int i = 0; i < 11; ++i) {
        printf("%c", bootStruct.BS_VolLab[i]);
    }
    printf("\n");
    printf("Deceptive FAT type Label: ");
    for (int i = 0; i < 8; ++i) {
        printf("%c", bootStruct.BS_FilSysType[i]);
    }
    printf("\n");

}


// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void PrintDirectoryEntry(DirEntry dirent)
{
    int i = 7;
    char p_string[64] = "              -----  00:00:00 03/01/2004";
    FATDate date;                                // The Date bit field structure
    FATTime time;                                // The Time bit field structure

    strncpy(p_string, (char*)&dirent.Name, 8);   // Copies 8 bytes from the name
    while (p_string[i] == ' ') i--;
    p_string[i+1] = '.';                         // Add extension
    strncpy(&p_string[i+2], (char*)&dirent.Extension, 3);
    while (p_string[i+4] == ' ') i--;
    if (p_string[i+4] == '.') p_string[i+4] = ' ';

    // Generate the attributes
    if(dirent.Attributes &0x01) p_string[14] = 'R';
    if(dirent.Attributes &0x02) p_string[15] = 'H';
    if(dirent.Attributes &0x04) p_string[16] = 'S';
    if(dirent.Attributes &0x08) p_string[17] = 'V';
    if(dirent.Attributes &0x10) p_string[18] = 'D';
    if(dirent.Attributes &0x20) p_string[19] = 'A';

    // Extract the time and format it into the string
    memcpy(&time, &dirent.Time, sizeof(FATTime));
    sprintf(&p_string[21], "%02d:%02d:%02d", time.hour, time.min, time.sec*2);

    // Extract the date and format it into the string
    memcpy(&date, &dirent.Date, sizeof(FATDate));
    sprintf(p_string, "%s %02d/%02d/%04d  %d  %d", p_string,
            date.month, date.day, date.year+1980,
            dirent.startCluster, dirent.fileSize);

    printf("\n%s", p_string);        // p_string is now ready!

} // end PrintDirectoryEntry


void printRootDir(int fd, uint32_t rootOffset){
    printf("----ROOT DIRECTORY----\n");
    lseek(fd, rootOffset, SEEK_SET);
    while (1){
        DirEntry dirEntry;
        read(fd, &dirEntry, sizeof(dirEntry));
        // No dirEntries after
        if (dirEntry.Name[0] == 0) {
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
        PrintDirectoryEntry(dirEntry);
    }
}


int main() {
    int fd;
    fd = open("../fat12_fs.img", O_RDONLY);

    BSStruct bootSector;       // The BYTE-aligned structure.
    read(fd, &bootSector, sizeof(bootSector));
    printBootSector(bootSector);

    uint32_t offset = (bootSector.BPB_RsvdSecCnt + bootSector.BPB_NumFATs*bootSector.BPB_FATSz16) * bootSector.BPB_BytsPerSec;
    printf("\n");
    printRootDir(fd, offset);
    close(fd);

    return 0;
}
