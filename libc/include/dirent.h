#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/syslimits.h>
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#pragma once

/**
 * @brief directory entry
 * 
 * 
 */
struct dirent {
    char d_name[_NAME_MAX]; /** long file name **/
    unsigned long d_size; /** file size **/
    unsigned long d_date; /** date year from 1980 (15:9), month (8:5), day (4:0) **/
    unsigned long d_time; /** time hour (15:11), minutes (10:5), seconds (4:0) **/
    unsigned long d_type; /** directory entry type **/
};

typedef struct _dir {
    struct dirent dirent;
} DIRe;

/**
 * @brief Open Directory path on device
 * 
 * @param path Path name to of directory
 * @return DIR* pointer to directory object
 */
DIR *opendir(const char *path);

/**
 * @brief Read the next directory entry
 * 
 * @param dirp pointer to directory object from opendir
 * @param ent pointer to a directory object to be filled in
 * @return int 0 on success errno set
 */
int readdir(DIR *dirp, struct dirent *ent);

/**
 * @brief Close directory object
 * 
 * @param dirp pointer to directory object that was opened
 * @return int 0 on success errno set
 */
int closedir(DIR *dirp);

#endif
