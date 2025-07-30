/**
 * @brief Interface Driver for SD cards
 * @author Michael Burmeister
 * @date Feburary 1, 2021
 * @version 1.0
 * 
*/

#pragma once

/* Integer types used for FatFs API */

#if defined(_WIN32x)	/* Main development platform */
#define FF_INTDEF 2
#include <windows.h>
typedef unsigned __int64 QWORD;
#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__cplusplus)	/* C99 or later */
#define FF_INTDEF 2
#include <stdint.h>
typedef unsigned int	UINT;	/* int must be 16-bit or 32-bit */
typedef unsigned char	BYTE;	/* char must be 8-bit */
typedef uint16_t		WORD;	/* 16-bit unsigned integer */
typedef uint32_t		DWORD;	/* 32-bit unsigned integer */
typedef uint64_t		QWORD;	/* 64-bit unsigned integer */
typedef WORD			WCHAR;	/* UTF-16 character type */
#else  	/* Earlier than C99 */
#define FF_INTDEF 1
typedef unsigned int	UINT;	/* int must be 16-bit or 32-bit */
typedef unsigned char	BYTE;	/* char must be 8-bit */
typedef unsigned short	WORD;	/* 16-bit unsigned integer */
typedef unsigned long	DWORD;	/* 32-bit unsigned integer */
typedef WORD			WCHAR;	/* UTF-16 character type */
#endif

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC3		0x01		/* MMC ver 3 */
#define CT_MMC4		0x02		/* MMC ver 4+ */
#define CT_MMC		0x03		/* MMC */
#define CT_SDC1		0x04		/* SDC ver 1 */
#define CT_SDC2		0x08		/* SDC ver 2+ */
#define CT_SDC		0x0C		/* SDC */
#define CT_BLOCK	0x10		/* Block addressing */

/**
 * @brief Transmit bytes to the card
 * 
 * @param buff: data to be sent
 * @param bc: number of bytes to send
 */
__attribute__((noinline)) void sd_xmit_mmc(const BYTE* buff, UINT bc);

/**
 * @brief Receive bytes from the card
 * 
 * @param buff: Pointer to read buffer
 * @param bc: Number of bytes to receive
 */
__attribute__((noinline)) void sd_rcvr_mmc(BYTE *buff, UINT bc);

/**
 * @brief wait for card ready
 * 
 * @returns 1 if OK, 0 if timeout
 */
int sd_wait_ready();

/**
 * @brief Deselect the card and release SPI bus 
 */
void sd_deselect();

/**
 * @brief Select the card and wait for ready 
 * 
 * @returns 1 if OK, 0 if timeout
 */
int sd_select();

/**
 * @brief Receive a data packet from the card
 * 
 * @param buff: Data buffer to store received data
 * @param btr: Byte count
 */
int sd_rcvr_datablock(BYTE *buff, UINT btr);

/**
 * @brief Send a data packet to the card 
 * 
 * @param buff: 512 byte data block to be transmitted
 * @param token: Data/Stop token
 * 
 * @returns 1 if OK, 0 if failed
 */
int sd_xmit_datablock(const BYTE *buff, BYTE token);

/**
 * @brief Send a command packet to the card
 * 
 * @param cmd: Command byte
 * @param arg: argument
 * @returns command response (bit7==1:Send failed)
 */
int sd_send_cmd(BYTE cmd, DWORD arg);

/**
 * @brief set the pins for the driver
 */
int sd_setpins(int drv, int pclk, int pss, int pdi, int pdo);

/**
 * @brief set up the driver hardware
 */
int sd_init();

/**
 * @brief clean up the driver hardware 
 */
int sd_deinit();



















// /**
//  * @brief Set SD Interface
//  * @param drive number
//  * @param cs chip select pint
//  * @param clk clock pin
//  * @param mosi master out slave in
//  * @param miso master in slave out
//  */
// void SetSD(int drive, int cs, int clk, int mosi, int miso);

// /**
//  * @brief Enable SD Interface
//  * @param drive number
//  */
// void EnableSD(int drive);

// /**
//  * @brief Send Data to SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer data
//  * @param bc number of bytes to send
//  */
// void SendSD(int drive, const BYTE *buff, unsigned int bc);

// /**
//  * @brief Receive Data from SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer data
//  * @param bc number of bytes to receive
//  */
// void ReceiveSD(int drive, BYTE *buff, unsigned int bc);

// /**
//  * @brief Send Data Block to SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer data
//  * @param bc number of bytes to send
//  */
// void SendSDB(int drive, const BYTE *buff, unsigned int bc);

// /**
//  * @brief Receive Data from SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer data
//  * @param bc number of bytes to receive
//  */
// void ReceiveSD(int drive, BYTE *buff, unsigned int bc);

// /**
//  * @brief Get ready state for SD Memory Card
//  * @param drive volume number 0/1
//  * @return ready ready status 0 - not ready
//  */
// int GetStatus(int drive);

// /**
//  * @brief Release SD Memory Card
//  * @param drive volume number 0/1
//  */
// void ReleaseSD(int drive);

// /**
//  * @brief Select SD Memory Card
//  * @param drive volume number 0/1
//  * @return status 0 - not ready
//  */
// int SelectSD(int drive);

// /**
//  * @brief Receive Block of data from SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer of data
//  * @param bc byte count to receive
//  * @return status 0 - not ready
//  */
// int ReceiveBlock(int drive, BYTE *buff, unsigned int bc);

// /**
//  * @brief Send Block of data to SD Memory Card
//  * @param drive volume number 0/1
//  * @param buff pointer to buffer of data
//  * @param token end of send data
//  * @return status 0 - not ready
//  */
// int SendBlock(int drive, const BYTE *buff, int token);

// /**
//  * @brief Send Command to SD Memory Card
//  * @param drive volume number 0/1
//  * @param cmd command to send
//  * @param arg command options
//  * @return status bit 7 == 1 failed
//  */
// BYTE SendCommand(int drive, const BYTE cmd, unsigned int arg);

// /**
//  * @brief Disk Status
//  * @param drive volume number 0/1
//  * @return status
//  */
// char MMC_disk_status(int drive);

// /**
//  * @brief Disk Initialize
//  * @param drive volume number 0/1
//  * @return status
//  */
// char MMC_disk_initialize(int drive);

