/**
 * @brief Flash Driver
 * @author Michael Burmeister
 * @date January 26, 2021
 * @version 1.0
 * 
*/

typedef int* flash_t;

/**
 * @brief Initalize Flash Driver
 * @param MOSI Master out slave in pin
 * @param MISO Master in slave out pin
 * @param CLK clock pin
 * @param CS chip select pin
 * @return flash_t pointer to flash
 */
flash_t* Flash_Init(int MOSI, int MISO, int CLK, int CS);

/**
 * @brief Flash reset
 * @param x flash pointer
 */
void Flash_Reset(flash_t *x);

/**
 * @brief Flash busy
 * @param x flash pointer
 * @return status 0 - not busy 1 - busy
 */
int Flash_Busy(flash_t *x);

/**
 * @brief Flash read
 * @param x flash pointer
 * @param address address to start read
 * @param buffer point to buffer
 * @param len number of bytes to read
 * @return bytes number of bytes read
 */
int Flash_Read(flash_t *x, int address, unsigned char *buffer, int len);

/**
 * @brief Flash write
 * @param x flash pointer
 * @param address address to start write
 * @param buffer pointer to data
 * @param len number of bytes to write
 * @return bytes number of bytes written
 */
int Flash_Write(flash_t *x, int address, unsigned char *buffer, int len);

/**
 * @brief Flash erase
 * @param x flash pointer
 * @param address address to start erase
 * @param len number of bytes to erase
 */
void Flash_Erase(flash_t *x, int address, int len);
