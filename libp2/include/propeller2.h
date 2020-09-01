#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "smartpins.h"

#define hubset(h) asm("hubset %0" : : "r"(h))
#define waitx(t) asm("waitx %0" : : "r"(t))
#define dirh(pin) asm("dirh %0" : : "r"(pin))
#define dirl(pin) asm("dirl %0" : : "r"(pin))
#define outh(pin) asm("outh %0" : : "r"(pin))
#define outl(pin) asm("outl %0" : : "r"(pin))

#define testp(pin, res) asm("testp %1 wc\n"     \
                            "if_c mov %0, #1\n"  \
                            "if_nc mov %0, #0" : "=r"(res) : "r"(pin))

#define rdpin(v, pin) asm("rdpin %0, %1" : "=r"(v) : "r"(pin))
#define rqpin(v, pin) asm("rqpin %0, %1" : "=r"(v) : "r"(pin))

#define wrpin(v, pin) asm("wrpin %0, %1" : : "r"(v), "r"(pin))
#define wxpin(v, pin) asm("wxpin %0, %1" : : "r"(v), "r"(pin))
#define wypin(v, pin) asm("wypin %0, %1" : : "r"(v), "r"(pin))

#define wrc(x) asm("wrc %0" : "=r"(x) :)
#define wrnc(x) asm("wrnc %0" : "=r"(x) :)
#define wrz(x) asm("wrz %0" : "=r"(x) :)
#define wrnz(x) asm("wrnz %0" : "=r"(x) :)

#define _clkfreq (*((int*)0x14))
#define _clkmode (*((int*)0x18))
#define _baudrate (*((int*)0x1c))

register volatile int PA asm ("pa");
register volatile int PB asm ("pb");
register volatile int PTRA asm ("ptra");
register volatile int PTRB asm ("ptrb");

register volatile int DIRA asm ("dira");
register volatile int DIRB asm ("dirb");
register volatile int OUTA asm ("outa");
register volatile int OUTB asm ("outb");
register volatile int INA asm ("ina");
register volatile int INB asm ("inb");

register volatile int IJMP3 asm ("ijmp3");
register volatile int IRET3 asm ("iret3");
register volatile int IJMP2 asm ("ijmp2");
register volatile int IRET2 asm ("iret2");
register volatile int IJMP1 asm ("ijmp1");
register volatile int IRET1 asm ("iret1");

#ifndef __weak_alias
#define __weak_alias(sym, oldfunc) extern __typeof (oldfunc) sym __attribute__ ((weak, alias (#oldfunc)));
#endif
#ifndef __strong_alias
#define __strong_alias(sym, oldfunc) extern __typeof (oldfunc) sym __attribute__ ((alias (#oldfunc)));
#endif

/* type for a volatile lock */
/* if we change this type, change the definitions of SIG_ATOMIC_{MIN,MAX}
 * in stdint.h too
 */
/* also note that in practice these have to go in HUB memory */
typedef volatile int _atomic_t;
typedef _atomic_t atomic_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * run clock configuration to the desired clock mode and clock frequency
 */
void _clkset(unsigned clkmode, unsigned clkfreq);

/*
 * return the current count
 */
unsigned int _cnt();

/*
 * wait until current count == cnt
 */
void _waitcnt(unsigned int cnt);

/*
 * start a new hub mode cog dictated by mode
 */
int _coginit(unsigned mode, void (*f)(void *), void *par) __attribute__((noinline));

/*
 * reverse bits in x
 */
unsigned int _rev(unsigned int x);

/*
 * initialize the given rx/tx pins in async mode (uart)
 * this needs to be called again if using UART and clkset is called to adjust baud rates
 */
unsigned _uart_init(unsigned rx, unsigned tx, unsigned baud);

/*
 * write a character to the builtin UART
 */
void _uart_putc(char c);

/*
 * read a character from the builtin UART
 */
char _uart_getc();

/*
 * request a new hardware lock
 */
unsigned int _locknew();

/*
 * return the lock to the pool
 */
void _lockret(unsigned int l);

/*
 * try to lock a lock, blocking until it is acquired.
 */
void _lock(atomic_t l);

/*
 * release the lock
 */
void _unlock(atomic_t l);

#ifdef __cplusplus
}
#endif

#endif