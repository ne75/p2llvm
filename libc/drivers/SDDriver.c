/**
 * @brief SD File System Driver Library
 */

#define _FILE_DEFINED

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include "ff.h"
#include "diskio.h"
#include "sd_mmc.h"
#include "../include/sys/sdcard.h"

//#define _DEBUG

#define FILE_BUFFER_SIZE 64

extern _Driver _SDDriver;

static int _set_dos_error(int);

struct _default_buffer {
    int cnt;
    unsigned char *ptr;
    unsigned flags;
#define _BUF_FLAGS_READING (0x01)
#define _BUF_FLAGS_WRITING (0x02)    
    unsigned char buf[1024];
};

typedef struct fat_file {
    //struct _default_buffer b;
    FIL fil;
} FAT_FIL;

static FATFS FatFs;

static const char sd_prefix[] = "SD:";
extern _Driver *_driverlist[];

int sd_mount(int cs, int clk, int mosi, int miso)
{
    int r;
    int i = 0;

    SetSD(cs, clk, mosi, miso);
    r = f_mount(&FatFs, "", 0);
    if (r != 0) {
#ifdef _DEBUG
       __builtin_printf("sd card fs_init failed: result=[%d]\n", r);
       _wait(1000);
#endif      
       _seterror(-r);
       return 0;
    }

    //Add sd driver to the list
    while (_driverlist[i] != NULL)
        i++;
    _driverlist[i] = &_SDDriver;

    return r;
}

static int _set_dos_error(int derr)
{
    int r;
#ifdef _DEBUG
    __builtin_printf("_set_dos_error(%d)\n", derr);
#endif    
    switch (derr) {
    case FR_OK:
        r = 0;
        break;
    case FR_NO_FILE:
    case FR_NO_PATH:
    case FR_INVALID_NAME:
        r = ENOENT;
        break;
    case FR_DENIED:
    case FR_WRITE_PROTECTED:
        r = EACCES;
        break;
    case FR_EXIST:
        r = EEXIST;
        break;
    case FR_NOT_ENOUGH_CORE:
        r = ENOMEM;
        break;
    case FR_INVALID_PARAMETER:
    case FR_INVALID_OBJECT:
    case FR_INVALID_DRIVE:
    case FR_NOT_ENABLED:
    case FR_NO_FILESYSTEM:
        r = EINVAL;
        break;
    case FR_TOO_MANY_OPEN_FILES:
        r = EMFILE;
        break;
        
    case FR_DISK_ERR:
    case FR_INT_ERR:
    case FR_NOT_READY:
    default:
        r = EIO;
        break;
    }
    return _seterror(r);
}

int sd_creat(FILE *fil, const char *pathname, mode_t mode)
{
  int r;
  FAT_FIL *f = malloc(sizeof(*f));
  int fatmode;
  
  if (!f) {
      return _seterror(ENOMEM);
  }
  memset(f, 0, sizeof(*f));
  fatmode = FA_CREATE_NEW | FA_WRITE | FA_READ;
  r = f_open(&f->fil, pathname, fatmode);
#ifdef _DEBUG
  __builtin_printf("sd_create(%s) returned %d\n", pathname, r);
#endif  
  if (r) {
    free(f);
    return _seterror(r);
  }
  fil->drvarg[0] = (unsigned long)f;
  return 0;
}

static int sd_close(FILE *fil)
{
    int r;
    FAT_FIL *f = (FAT_FIL *)fil->drvarg[0];
    r=f_close(&f->fil);
    if ((fil->_flag & _IOFREEBUF))
    {
        free(fil->_base);
        fil->_base = fil->_ptr = NULL;
        fil->_bsiz = 0;
        fil->_flag = 0;
    }
    free(f);
    return _set_dos_error(r);
}

DIR *sd_opendir(const char *name)
{
    DIR *f = malloc(sizeof(*f));
    int r;

#ifdef _DEBUG    
    __builtin_printf("sd_opendir(%s)\n", name);
#endif    
    if (!f) {
#ifdef _DEBUG
      __builtin_printf("malloc failed\n");
#endif
      _seterror(ENOMEM);
      return NULL;
    }
    r = f_opendir(f, name);
#ifdef _DEBUG
    __builtin_printf("f_opendir(%s) returned %d\n", name, r);
#endif    
    
    if (r) {
        free(f);
        _set_dos_error(r);
        return NULL;
    }
    return f;
}

int sd_closedir(DIR *dir)
{
    int r;

    r = f_closedir(dir);
    free(dir);
    if (r) _set_dos_error(r);
    return r;
}

int sd_readdir(DIR *dir, struct dirent *ent)
{
    FILINFO finfo;
    int r;

    r = f_readdir(dir, &finfo);
#ifdef _DEBUG       
    __builtin_printf("readdir fs_read: %d\n", r);
#endif	
    if (r != 0) {
        return _set_dos_error(r); // error
    }
    if (finfo.fname[0] == 0) {
        return -1; // EOF
    }
#if FF_USE_LFN
    strncpy(ent->d_name, finfo.fname, _NAME_MAX-1);
    ent->d_name[_NAME_MAX-1] = 0;
#else
    strcpy(ent->d_name, finfo.fname);
#endif
    ent->d_type = finfo.fattrib;
    ent->d_date = finfo.fdate;
    ent->d_time = finfo.ftime;
    return 0;
}

static time_t unixtime(unsigned int dosdate, unsigned int dostime)
{
    time_t t;
    //unsigned year = (dosdate >> 9) & 0x7f;
    //unsigned month = ((dosdate >> 5) & 0xf) - 1;
    //unsigned day = (dosdate & 0x1f) - 1;
    unsigned hour = (dostime >> 11) & 0x1f;
    unsigned minute = (dostime >> 5) & 0x3f;
    unsigned second = (dostime & 0x1f) << 1;

    t = second + minute*60 + hour * 3600;
    return t;
}

int sd_stat(const char *name, struct stat *buf)
{
    int r;
    FILINFO finfo;
    unsigned mode;
#ifdef _DEBUG
    __builtin_printf("sd_stat(%s)\n", name);
#endif
    memset(buf, 0, sizeof(*buf));
    if (name[0] == 0 || (name[0] == '.' && name[1] == 0)) {
        /* root directory */
        finfo.fattrib = AM_DIR;
        r = 0;
    } else {
        r = f_stat(name, &finfo);
    }
    if (r != 0) {
        return _set_dos_error(r);
    }
    mode = _IOREAD | _IOWRT | _IORW;
    if (finfo.fattrib & AM_RDO) {
        mode |= _IOREAD;
    }
    if (finfo.fattrib & AM_DIR) {
        mode |= S_IFDIR;
    }
    buf->st_mode = mode;
    buf->st_nlink = 1;
    buf->st_size = finfo.fsize;
    //buf->st_blksize = 512;
    //buf->st_blocks = buf->st_size / 512;
    buf->st_atime = buf->st_mtime = buf->st_ctime = unixtime(finfo.fdate, finfo.ftime);
#ifdef _DEBUG
    __builtin_printf("sd_stat returning %d\n", r);
#endif
    return r;
}

static int sd_open(FILE *fil, const char *name, const char *flags)
{
  int r;
  FAT_FIL *f = malloc(sizeof(*f));
  unsigned char *buf;
  unsigned fs_flags;

  if (!f) {
      return _seterror(ENOMEM);
  }
  memset(f, 0, sizeof(*f));
  buf = malloc(FILE_BUFFER_SIZE);

  if (flags[1] == '+')
    fs_flags = FA_READ | FA_WRITE;
  if (flags[0] == 'r')
    fs_flags = FA_READ;
  if (flags[0] == 'w')
    fs_flags = FA_WRITE | FA_CREATE_ALWAYS;
  if (flags[0] == 'a')
    fs_flags = FA_WRITE | FA_OPEN_APPEND;

#ifdef _DEBUG
    __builtin_printf("sd_open %s\n", name);
#endif
  r = f_open(&f->fil, name, fs_flags);
  if (r) {
    free(buf);
    free(f);
    return _set_dos_error(r);
  }
  fil->_ptr = fil->_base = buf;
  fil->_bsiz = FILE_BUFFER_SIZE;
  fil->_flag |= _IOFREEBUF;
  fil->drvarg[0] = (unsigned long)f;
  return 0;
}

static int sd_read(FILE *fil, unsigned char *buf, int siz)
{
    FAT_FIL *f = (FAT_FIL *)fil->drvarg[0];
    int r;
    UINT x;
    
    if (!f) {
        return _seterror(EBADF);
    }
#ifdef _DEBUG
    __builtin_printf("sd_read: f_read of %u bytes:", siz);
#endif    
    r = f_read(&f->fil, buf, siz, &x);
#ifdef _DEBUG
    __builtin_printf(" ...returned %d\n", r);
    __builtin_printf("err: %d, data: %d\n", f->fil.err, x);
#endif    
    if (r != 0) {
        //fil->state |= _VFS_STATE_ERR;
        return _set_dos_error(r);
    }
    if (x == 0) {
        //fil->state |= _VFS_STATE_EOF;
    }
    return x;
}

static int sd_write(FILE *fil, unsigned char *buf, int siz)
{
    FAT_FIL *f = (FAT_FIL *)fil->drvarg[0]; // get ff saved information
    int r;
    UINT x;
    if (!f) {
        return _seterror(EBADF);
    }
#ifdef _DEBUG    
    __builtin_printf("sd_write: f_write %d bytes:", siz);
#endif    
    r = f_write(&f->fil, buf, siz, &x);
#ifdef _DEBUG
    __builtin_printf("returned %d\n", r);
#endif    
    if (r != 0) {
        //fil->state |= _VFS_STATE_ERR;
        return _set_dos_error(r);
    }
    return x;
}

static int sd_seek(FILE *fil, long int offset, int whence)
{
    FAT_FIL *vf = (FAT_FIL *)fil->drvarg[0];  //get ff saved information
    FIL *f = &vf->fil;
    int result;
    
    if (!f) {
        return _set_dos_error(EBADF);
    }
#ifdef _DEBUG
    __builtin_printf("sd_lseek(%ld, %d) ", offset, whence);
#endif    
    if (whence == SEEK_SET) {
        /* offset is OK */
    } else if (whence == SEEK_CUR) {
        offset += f->fptr;
    } else {
        offset += f->obj.objsize;
    }
    result = f_lseek(f, offset);
#ifdef _DEBUG
    __builtin_printf("result=%d\n", result);
#endif
    if (result) {
        return _set_dos_error(result);
    }
    return offset;
}

int sd_ioctl(FILE *fil, unsigned long req, void *argp)
{
    return _seterror(EINVAL);
}

int sd_mkdir(const char *name, mode_t mode)
{
    int r;

    r = f_mkdir(name);
    return _set_dos_error(r);
}

int sd_remove(const char *name)
{
    int r;

    r = f_unlink(name);
    return _set_dos_error(r);
}

int sd_rmdir(const char *name)
{
    int r;

    r = f_unlink(name);
    return _set_dos_error(r);
}

int sd_rename(const char *old, const char *new)
{
    int r = f_rename(old, new);
    return _set_dos_error(r);
}

// Define the driver list
_Driver _SDDriver =
{
    sd_prefix,
    sd_open,
    sd_close,
    sd_read,
    sd_write,
    sd_seek,
    sd_remove,
};

DWORD get_fattime()
{
    int t;
    struct tm *x;
    time_t w;
    
    time(&w);
    x = localtime(&w);
    
    t = x->tm_year - 80; //adjust for 1980
    t = (t << 4) | (x->tm_mon + 1);
    t = (t << 5) | x->tm_mday;
    t = (t << 5) | x->tm_hour;
    t = (t << 6) | x->tm_min;
    t = (t << 5) | x->tm_sec;
    
    return t;
}
