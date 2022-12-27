#include <cstring>
#include <iostream>
#include "dir_entry.hpp"
#include "fats.hpp"
#include "fats_versions.hpp"

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
    if(dirent->Attributes &READ_ONLY) p_string[14] = 'R';
    if(dirent->Attributes &HIDDEN ) p_string[15] = 'H';
    if(dirent->Attributes &SYSTEM) p_string[16] = 'S';
    if(dirent->Attributes &VOLUME) p_string[17] = 'V';
    if(dirent->Attributes &DIRECTORY) p_string[18] = 'D';
    if(dirent->Attributes &ARCHIVE) p_string[19] = 'A';

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

int check_cluster_index(uint32_t cluster_index, int file_type) {
    if (file_type & FAT12) {
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

        return 1;
    }

    if (file_type & FAT16) {
        if (cluster_index < 2) {
            printf("\nWrong cluster index!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if ((cluster_index <= 0xFFF6) && (cluster_index >= 0xFFF0)) {
            printf("\nReserved cluster index!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if (cluster_index == 0xFFF7) {
            printf("\nBad cluster!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if ((cluster_index <= 0xFFFF) && (cluster_index >= 0xFFF8)) {
            return 0;
        }

        return 1;
    }
}

int check_DirEntry_helper(uint32_t cluster_index, BSStruct *bootSector, int total_clusters, int fd,
                          unsigned char FAT[], int cluster_size, int offset, int fat_type,
                          std::unordered_set <uint32_t>* hash_table) {

    int check_res = check_cluster_index(cluster_index, fat_type);
    if (check_res <= 0) {
        return check_res;
    }

    if (cluster_index > total_clusters - 1) {
        printf("\nInvalid cluster index!!! : 0x%x\n", cluster_index);
        return -1;
    }

    if (hash_table->count(cluster_index) > 0) {
        printf("\nCluster chain is looped, looped cluster: %i!!!\n", cluster_index);
        return -1;
    }

    hash_table->insert(cluster_index);

    unsigned short new_cluster_index = GetFatEntry(cluster_index, FAT, fat_type);

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
    int other_bytes = check_DirEntry_helper(new_cluster_index, bootSector, total_clusters,
                                            fd, FAT, cluster_size, offset, fat_type, hash_table);
    if (other_bytes >= 0) {
        return read_bytes + other_bytes;
    } else {
        return -1;
    }
}

int check_DirEntry(DirEntry *dirent, BSStruct *bootSector, int fd, unsigned char FAT[], int fat_type) {
    int cluster_size = bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec;
    int root_dir_sectors = ((bootSector->BPB_RootEntCnt * 32) + (bootSector->BPB_BytsPerSec - 1)) / bootSector->BPB_BytsPerSec;
    int first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors;  // FAT-12 там де число 16
    int data_sectors = bootSector->BPB_TotSec16 - (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors); // FAT-12 там де число 16
    int total_clusters = data_sectors / bootSector->BPB_SecPerClus;

    printf("\nStart Cluter: %i", dirent->startCluster);
    int offset = (first_data_sector + (dirent->startCluster - 2) * bootSector->BPB_SecPerClus) * bootSector->BPB_BytsPerSec;
    printf("\nCurr position: %x", offset);
    lseek(fd, offset, SEEK_SET);
    std::unordered_set <uint32_t> hash_table;
    int real_size = check_DirEntry_helper(dirent->startCluster, bootSector, total_clusters, fd, FAT,
                                          cluster_size, offset, fat_type, &hash_table);

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