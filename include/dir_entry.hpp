#ifndef CHECKING_FAT_DIR_ENTRY_H
#define CHECKING_FAT_DIR_ENTRY_H
#define N_FILE       0x00
#define READ_ONLY    0x01
#define HIDDEN       0x02
#define SYSTEM       0x04
#define VOLUME       0x08 // this is the volume label entry
#define DIRECTORY    0x10
#define ARCHIVE      0x20 // same as file

#include <unordered_set>
#include <csignal>
#include "structs.hpp"
#include "fats_versions.hpp"
#include "fats.hpp"

void PrintDirectoryEntry(DirEntry *dirent);

int check_cluster_index(uint32_t cluster_index, int file_type);

template<
        typename T ,
        typename = typename std::enable_if<
                std::is_base_of<BSStruct, T>::value ||
                std::is_base_of<BSStruct32 , T>::value
        >::type
>
long check_DirEntry_helper(uint32_t cluster_index, T *bootSector, size_t total_clusters, int fd,
                          unsigned char FAT[], int cluster_size, off64_t offset, int fat_type,
                          std::unordered_set <uint32_t>* hash_table) {

//    printf("\nCluster:%u", cluster_index);
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

    uint32_t new_cluster_index = GetFatEntry(cluster_index, FAT, fat_type);

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
    long other_bytes = check_DirEntry_helper(new_cluster_index, bootSector, total_clusters,
                                            fd, FAT, cluster_size, offset, fat_type, hash_table);
    if (other_bytes >= 0) {
        return read_bytes + other_bytes;
    } else {
        return -1;
    }
}

template<
        typename T ,
        typename = typename std::enable_if<
                std::is_base_of<BSStruct, T>::value ||
                std::is_base_of<BSStruct32 , T>::value
        >::type
>
int check_DirEntry(DirEntry *dirent, T *bootSector, int fd, unsigned char FAT[], int fat_type) {
    int cluster_size = bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec;
    size_t first_data_sector;
    size_t data_sectors;
    size_t total_clusters;
    if ( (fat_type &FAT12) || (fat_type &FAT16)) {
        size_t root_dir_sectors = ((bootSector->BPB_RootEntCnt * 32) + (bootSector->BPB_BytsPerSec - 1)) / bootSector->BPB_BytsPerSec;
        first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors;
        data_sectors = bootSector->BPB_TotSec16 - (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16) + root_dir_sectors);
        total_clusters = data_sectors / bootSector->BPB_SecPerClus;
    } else if (fat_type &FAT32) {
        first_data_sector = bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz32);
        data_sectors = bootSector->BPB_TotSec32 - (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz32));
        total_clusters = data_sectors / bootSector->BPB_SecPerClus;
    } else {
        std::cerr << "Unsupported FAT type!" << std::endl;
        exit(-1);
    }

//    printf("\nStart Cluter: %i", dirent->startCluster);
    off64_t offset = (first_data_sector + (dirent->startCluster - 2) * bootSector->BPB_SecPerClus) * bootSector->BPB_BytsPerSec;
//    printf("\nCurr position: %x", offset);
    lseek(fd, offset, SEEK_SET);
    std::unordered_set <uint32_t> hash_table;

    long real_size = check_DirEntry_helper(dirent->startCluster, bootSector, total_clusters, fd, FAT,
                                          cluster_size, offset, fat_type, &hash_table);

    if (real_size != dirent->fileSize){
        if (real_size >= 0) {
            printf("\nReal size: %u\n", real_size);
            printf("\nWrong size!!!\n");
        }
        return 1;
    } else {
        printf("\nReal size: %u\n", real_size);
        printf("\nSuccess!!!\n");
        return 0;
    }

}
#endif //CHECKING_FAT_DIR_ENTRY_H
