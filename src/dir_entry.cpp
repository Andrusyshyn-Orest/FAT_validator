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
    } else if (file_type & FAT16) {
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
    } else if (file_type & FAT32) {
        if (cluster_index < 2) {
            printf("\nWrong cluster index!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if ((cluster_index <= 0x0FFFFFF6) && (cluster_index >= 0x0FFFFFF0)) {
            printf("\nReserved cluster index!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if (cluster_index == 0x0FFFFFF7) {
            printf("\nBad cluster!!! : 0x%x\n", cluster_index);
            return -1;
        }

        if ((cluster_index <= 0x0FFFFFFF) && (cluster_index >= 0x0FFFFFF8)) {
            return 0;
        }

        return 1;
    } else {
        std::cerr << "Unsupported FAT type!" << std::endl;
        exit(-1);
    }
}
