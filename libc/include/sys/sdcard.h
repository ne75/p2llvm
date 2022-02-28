/**
 * @file sdcard.h
 * @brief SD Card System glue code
 * 
 * This code maps the sd card functions to the normal
 * function names for convience
 */
#include "../../drivers/ff.h"
#include <dirent.h>
#include <sys/stat.h>


/**
 * @brief mount sd card file system
 * @param cs chip select pin
 * @param clk clock pin
 * @param mosi master out slave in pin
 * @param miso master in slave out pin
 * @return results
 */
int sd_mount(int cs, int clk, int mosi, int miso);

/**
 * @brief open directory of sd card
 * @param name directory to start at "sd:/"
 * @return pointer to the directory object used for reading
 */
DIR *sd_opendir(const char *name);

/**
 * @brief close directory prevously opened
 * @param dir pointer to directory object that was opened
 * @return return 0 on close
 */
int sd_closedir(DIR *dir);

/**
 * @brief read directory object and return next entry
 * @param dirp pointer to directory object return by opendir
 * @param ent struct of directory entry pointer
 * @return return 0 on EOF
 */
int sd_readdir(DIR *dirp, struct dirent *ent);

/**
 * @brief create emty file
 * @param fp file pointer of created file
 * @param pathname location of file to create
 * @param mode file mode of newly created file
 * @return return 0 on success
 */
int sd_creat(FILE *fp, const char *pathname, mode_t mode);

/**
 * @brief get file information
 * @param name path name of file
 * @param s pointer to file information structure
 * @return 0 on success
 */
int sd_stat(const char *name, struct stat *s);

/**
 * @brief make directory entry
 * 
 * @param name path name of directory
 * @param mode not used
 * @return int 0 on success errno set
 */
int sd_mkdir(const char *name, mode_t mode);

/**
 * @brief remove file from sd card
 * 
 * @param name path name to file
 * @return int 0 on success errno set 
 */
int sd_remove(const char *name);

/**
 * @brief remove directory entry from sd
 * 
 * @param name path name to directory
 * @return int 0 on success errno set
 */
int sd_rmdir(const char *name);

/**
 * @brief rename a file entry
 * 
 * @param old path to old file
 * @param new new file name
 * @return int 0 on success errno set
 */
int sd_rename(const char *old, const char *new);

