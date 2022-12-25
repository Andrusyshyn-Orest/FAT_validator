#ifndef CHECKING_FAT_STRUCTS_H
#define CHECKING_FAT_STRUCTS_H
#include <stdint-gcc.h>

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


#endif //CHECKING_FAT_STRUCTS_H
