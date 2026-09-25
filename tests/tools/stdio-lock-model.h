// Native-only replacement for P2 registers and the sixteen hardware locks.
#ifndef P2_STDIO_LOCK_MODEL_H
#define P2_STDIO_LOCK_MODEL_H
#define __propeller2__
#define _PROPELLER_H
#define _PROPELLER2_H
#define _PROP1_COMPATIBLE
typedef volatile int _atomic_t;
void *memset(void *, int, __SIZE_TYPE__);
unsigned _locknew(void);
void _lockret(unsigned);
void _lock(unsigned);
int _locktry(unsigned);
void _unlock(unsigned);
#endif
