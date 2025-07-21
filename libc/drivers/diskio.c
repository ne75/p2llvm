/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

// #define _DEBUG
// #define _DEBUG_ERROR
// #define _DEBUG_SDMM

#define WAITS asm("augd #3\nwaitx #464\n")

#include "sys/ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdmmc.h"

DSTATUS Stat /*= STA_NOINIT*/;	/* Disk status */
static char CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv) return STA_NOINIT;
    
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

// TODO: parse out pin set up to the sdmmc driver
DSTATUS disk_initialize(BYTE pdrv) {
	if (pdrv) { // drv must be 0
#ifdef _DEBUG_SDMM
		__builtin_printf("deinitialize: bad drv %d\n", pdrv);
#endif
		return RES_NOTRDY;
	}

    Stat = sd_init(&CardType);
	
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE drv, BYTE *buff, LBA_t sector, unsigned int count) {
	BYTE cmd;
	DWORD sect = (DWORD)sector;

#ifdef _DEBUG
        __builtin_printf("disk_read\n");
#endif	

	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	if (!(CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	cmd = count > 1 ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	if (sd_send_cmd(cmd, sect) == 0) {
		do {
			if (!sd_rcvr_datablock(buff, 512)) break;
			buff += 512;
		} while (--count);
		if (cmd == CMD18) sd_send_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
	}

	sd_deselect();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE drv, const BYTE *buff,	LBA_t sector, UINT count) {
	DWORD sect = (DWORD)sector;

	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	if (!(CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((sd_send_cmd(CMD24, sect) == 0)	/* WRITE_BLOCK */
			&& sd_xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) sd_send_cmd(ACMD23, count);
		if (sd_send_cmd(CMD25, sect) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!sd_xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!sd_xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	sd_deselect();

	return count ? RES_ERROR : RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

static inline LBA_t disk_size(uint8_t *csd) {
    uint32_t  cs = __builtin_bswap32(*(uint32_t*)(&csd[6]));
    unsigned n;
    if( csd[0]>>6 == 1) { // SDC ver 2.00
        cs = (cs & 0x3fffff)+1;
        n = 10;
    } else {    // SDC ver 1.00
        cs = (LBA_t)(cs>>14 & 0xfff)+1;
        n = (csd[5] & 15) + (__builtin_bswap32(*(uint16_t*)(&csd[9]))>>23 & 0x7)+(2-9);
    }
    return (LBA_t)cs << n;    // 32/64-bit block count
}

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff) {
	DRESULT res;
	BYTE csd[16];

	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;	/* Check if card is in the socket */

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			if (sd_select()) res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((sd_send_cmd(CMD9, 0) == 0) && sd_rcvr_datablock(csd, 16)) {
               	*(LBA_t*)buff = disk_size(csd);
				res = RES_OK;
			}
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			*(DWORD*)buff = 128;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	sd_deselect();

	return res;
}

// move this
DSTATUS disk_deinitialize( BYTE drv ) {
	if (drv) {
#ifdef _DEBUG_SDMM
		__builtin_printf("deinitialize: bad drv %d\n", drv);
#endif
		return RES_NOTRDY;
	}

	return sd_deinit();
}