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
#include "structs.hpp"

void PrintDirectoryEntry(DirEntry *dirent);

int check_cluster_index(uint32_t cluster_index, int file_type);

int check_DirEntry_helper(uint32_t cluster_index, BSStruct *bootSector, int total_clusters, int fd,
                          unsigned char FAT[], int cluster_size, int offset, int fat_type,
                          std::unordered_set <uint32_t>* hash_table);

int check_DirEntry(DirEntry *dirent, BSStruct *bootSector, int fd, unsigned char FAT[], int fat_type);
#endif //CHECKING_FAT_DIR_ENTRY_H