/**
 *
 * @brief map sd card functions to normal names
 */
 
 #include <stdio.h>
 #include <propeller.h>

/**
 * @brief mount an SD card
 * 
 * @param cs chip select pin
 * @param clk clock pin
 * @param mosi master out slave in pin
 * @param miso master in slave out pin
 * @return int 0 on success errno set
 */
 int mount(int cs, int clk, int mosi, int miso)
 {
    return sd_mount(cs, clk, mosi, miso);
 }

/**
 * @brief create a file on SD card
 * 
 * @param fp file pointer of created file
 * @param pathname file path name to create
 * @param mode file create mode
 * @return int 0 on success errno set
 */
int creat(FILE *fp, const char *pathname, mode_t mode)
{
    return sd_create(fp, pathname, mode);
}

/**
 * @brief get file information
 * 
 * @param name file path name
 * @param s pointer to struct of stat information
 * @return int 0 on success errno set
 */
int stat(const char *name, struct stat *s)
{
    return sd_stat(name, s);
}
