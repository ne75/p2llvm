#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "smartpins.h"
#include "streamer.h"

#define hubset(h) asm("hubset %0" : : "r"(h))
#define waitx(t) asm("waitx %0" : : "r"(t))
#define dirh(pin) asm("dirh %0" : : "r"(pin))
#define dirl(pin) asm("dirl %0" : : "r"(pin))
#define outh(pin) asm("outh %0" : : "r"(pin))
#define outl(pin) asm("outl %0" : : "r"(pin))
#define outnot(pin) asm("outnot %0" : : "r"(pin))

#define testp(pin, res) asm("testp %1 wc\nwrc %0\n" : "=r"(res) : "r"(pin))

#define rdpin(v, pin) asm("rdpin %0, %1" : "=r"(v) : "r"(pin))
#define rqpin(v, pin) asm("rqpin %0, %1" : "=r"(v) : "r"(pin))

#define wrpin(v, pin) asm("wrpin %0, %1" : : "r"(v), "r"(pin))
#define wxpin(v, pin) asm("wxpin %0, %1" : : "r"(v), "r"(pin))
#define wypin(v, pin) asm("wypin %0, %1" : : "r"(v), "r"(pin))

#define wrc(x) asm("wrc %0" : "=r"(x) :)
#define wrnc(x) asm("wrnc %0" : "=r"(x) :)
#define wrz(x) asm("wrz %0" : "=r"(x) :)
#define wrnz(x) asm("wrnz %0" : "=r"(x) :)

#define xinit(x, y) asm("xinit %0, %1" : : "r"(x), "r"(y))
#define setxfrq(x) asm("setxfrq %0" : : "r"(x))
#define rdfast(x, y) asm("rdfast %0, %1" : : "r"(x), "r"(y))
#define wrfast(x, y) asm("wrfast %0, %1" : : "r"(x), "r"(y))

#define wrlut(x, addr) asm("wrlut %0, %1" : : "r"(x), "r"(addr))
#define rdlut(x, addr) asm("rdlut %0, %1" : "=r"(x), : "r"(addr))

#define _clkfreq (*((int*)0x14))
#define _clkmode (*((int*)0x18))
#define _baudrate (*((int*)0x1c))

register volatile int R0 asm ("r0");
register volatile int R1 asm ("r1");
register volatile int R2 asm ("r2");
register volatile int R3 asm ("r3");
register volatile int R4 asm ("r4");
register volatile int R5 asm ("r5");
register volatile int R6 asm ("r6");
register volatile int R7 asm ("r7");
register volatile int R8 asm ("r8");
register volatile int R9 asm ("r9");
register volatile int R10 asm ("r10");
register volatile int R11 asm ("r11");
register volatile int R12 asm ("r12");
register volatile int R13 asm ("r13");
register volatile int R14 asm ("r14");
register volatile int R15 asm ("r15");
register volatile int R16 asm ("r16");
register volatile int R17 asm ("r17");
register volatile int R18 asm ("r18");
register volatile int R19 asm ("r19");
register volatile int R20 asm ("r20");
register volatile int R21 asm ("r21");
register volatile int R22 asm ("r22");
register volatile int R23 asm ("r23");
register volatile int R24 asm ("r24");
register volatile int R25 asm ("r25");
register volatile int R26 asm ("r26");
register volatile int R27 asm ("r27");
register volatile int R28 asm ("r28");
register volatile int R29 asm ("r29");
register volatile int R30 asm ("r30");
register volatile int R31 asm ("r31");

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

#define COG_PARAM_TO_OBJ(p, type) ((type*)((int*)p)[0]);

__attribute__ ((section ("cog"))) void __unreachable();

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

#define COGINIT_MODE_COG 0x10
#define COGINIT_MODE_HUB 0x00

/**
 * run clock configuration to the desired clock mode and clock frequency
 */
void _clkset(unsigned clkmode, unsigned clkfreq);

/**
 * return the current count
 */
unsigned int _cnt();

/**
 * wait until current count == cnt
 */
void _waitcnt(unsigned int cnt);

/**
 * start a new cog dictated by mode
 */
int _coginit(unsigned mode, void (*f)(void *), void *par) __attribute__((noinline));

/**
 * reverse bits in x
 */
unsigned int _rev(unsigned int x);

/**
 * initialize the given rx/tx pins in async mode (uart)
 * this needs to be called again if using UART and clkset is called to adjust baud rates
 */
unsigned _uart_init(unsigned rx, unsigned tx, unsigned baud);

/**
 * write a character to the builtin UART
 */
void _uart_putc(char c);

/**
 * read a character from the builtin UART
 */
char _uart_getc();

/**
 * request a new hardware lock
 */
unsigned int _locknew();

/**
 * return the lock to the pool
 */
void _lockret(unsigned int l);

/**
 * try to lock a lock, returning 1 if it succeed, 0 if not.
 */
int _locktry(atomic_t l);

/**
 * try to lock a lock, blocking until it is acquired.
 */
void _lock(atomic_t l);

/**
 * release the lock
 */
void _unlock(atomic_t l);

#ifdef __cplusplus
}
#endif

#endif