#include <stdio.h>
#include <string.h>
#include "dir_entry.h"
#include "fats.h"

// this function was taken from https://students.cs.byu.edu/~cs345ta/labs/P6-FAT%20Supplement.html
void PrintDirectoryEntry(DirEntry *dirent)
{
    int i = 7;
    char p_string[64] = "              -----  00:00:00 03/01/2004";
    FATDate date;                                // The Date bit field structure
    FATTime time;                                // The Time bit field structure

    strncpy(p_string, (char*)&dirent->Name, 8);   // Copies 8 bytes from the name
    while (p_string[i] == ' ') i--;
    p_string[i+1] = '.';                         // Add extension
    strncpy(&p_string[i+2], (char*)&dirent->Extension, 3);
    while (p_string[i+4] == ' ') i--;
    if (p_string[i+4] == '.') p_string[i+4] = ' ';

    // Generate the attributes
    if(dirent->Attributes &0x01) p_string[14] = 'R';
    if(dirent->Attributes &0x02) p_string[15] = 'H';
    if(dirent->Attributes &0x04) p_string[16] = 'S';
    if(dirent->Attributes &0x08) p_string[17] = 'V';
    if(dirent->Attributes &0x10) p_string[18] = 'D';
    if(dirent->Attributes &0x20) p_string[19] = 'A';

    // Extract the time and format it into the string
    memcpy(&time, &dirent->Time, sizeof(FATTime));
    sprintf(&p_string[21], "%02d:%02d:%02d", time.hour, time.min, time.sec*2);

    // Extract the date and format it into the string
    memcpy(&date, &dirent->Date, sizeof(FATDate));
    sprintf(p_string, "%s %02d/%02d/%04d  %d  %d", p_string,
            date.month, date.day, date.year+1980,
            dirent->startCluster, dirent->fileSize);

    printf("\n%s", p_string);        // p_string is now ready!

} // end PrintDirectoryEntry

int check_DirEntry_helper(unsigned short cluster_index, BSStruct *bootSector, int bit_map, int total_clusters, int fd, unsigned char FAT[], int cluster_size, int offset) {
    if (cluster_index < 2) {
        printf("\nWrong cluster index!!! : 0x%x\n", cluster_index);
        return -1;
    }

    if ((cluster_index <= 0xFF6) && (cluster_index >= 0xFF0)) {
        printf("\nReserved cluster index!!! : 0x%x\n", cluster_index);
        return -1;
    }

    if (cluster_index == 0xFF7) {
        printf("\nBad cluster!!! : 0x%x\n", cluster_index);
        return -1;
    }

    if ((cluster_index <= 0xFFF) && (cluster_index >= 0xFF8)) {
        return 0;
    }

    if (cluster_index > total_clusters - 1) {
        printf("\nInvalid cluster index!!! : 0x%x\n", cluster_index);
        return -1;
    }

    int new_bit_map = bit_map ^ (1 << cluster_index);

    if (new_bit_map < bit_map) {
        printf("\nCluster chain is looped!!!\n");
        return -1;
    }

    unsigned short new_cluster_index = GetFatEntry(cluster_index, FAT);


    char read_data[cluster_size];
    if (read(fd, read_data, cluster_size) < 0) {
        printf("\nError in read!!!\n");
    }
    int read_bytes = 0;
    for (int i = 0; i < cluster_size; ++i) {
        if (read_data[i] != 0x00) {
            read_bytes++;
        }
    }

    offset = offset + (new_cluster_index - cluster_index) * bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec;
    lseek(fd, offset, SEEK_SET);
    int other_bytes = check_DirEntry_helper(new_cluster_index, bootSector, new_bit_map, total_clusters, fd, FAT, cluster_size, offset);
    if (other_bytes >= 0) {
        return read_bytes + other_bytes;
    } else {
        return -1;
    }
}

int check_DirEntry(DirEntry *dirent, BSStruct *bootSector, int fd, unsigned char FAT[]) {
    int cluster_size = bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec;
    int root_dir_sectors = ((bootSector->BPB_RootEntCnt * 32) + (bootSector->BPB_BytsPerSec - 1)) / bootSector->BPB_BytsPerSec;
    int first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors;  // FAT-12 там де число 16
    int data_sectors = bootSector->BPB_TotSec16 - (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors); // FAT-12 там де число 16
    int total_clusters = data_sectors / bootSector->BPB_SecPerClus;
    int bit_map = 0;

//    printf("\nStart Cluter: %i", dirent->startCluster);
//    printf("\nCurr position: %i", (first_data_sector + dirent->startCluster) * bootSector->BPB_BytsPerSec);
    int offset = (first_data_sector + (dirent->startCluster - 2) * bootSector->BPB_SecPerClus) * bootSector->BPB_BytsPerSec;
    lseek(fd, offset, SEEK_SET);
    int real_size = check_DirEntry_helper(dirent->startCluster, bootSector, bit_map, total_clusters, fd, FAT, cluster_size, offset);

    printf("\n%i\n", real_size);
    if (real_size != dirent->fileSize){
        if (real_size >= 0) {
            printf("\nWrong size!!!\n");
        }
        return 1;
    } else {
        printf("\nSuccess!!!\n");
        return 0;
    }

}