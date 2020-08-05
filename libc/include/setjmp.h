#ifndef _SETJMP_H
#define _SETJMP_H

#include <sys/jmpbuf.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef _jmp_buf jmp_buf;

// void longjmp(jmp_buf env, int val);
// int  setjmp(jmp_buf env);

#define longjmp __builtin_longjmp
#define setjmp __builtin_setjmp

#if defined(__cplusplus)
}
#endif

#endif
