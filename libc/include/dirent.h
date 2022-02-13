#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/limits.h>

#pragma once

struct dirent {
    char d_name[_NAME_MAX];
    unsigned long d_off;
    unsigned long d_ino;
    unsigned long d_type;
};

typedef struct _dir {
    struct dirent dirent;
} DIRe;

#endif
