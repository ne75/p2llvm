#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "smartpins.h"
#include "streamer.h"

#define DBG_UART_RX_PIN 63
#define DBG_UART_TX_PIN 62
#define DBG_UART_BAUD 3000000

#define E_IN_RISE   (1 << 6)
#define E_IN_FALL   (2 << 6)
#define E_IN_CHANGE (3 << 6)
#define E_IN_LOW    (4 << 6)
#define E_IN_HIGH   (6 << 6)

// assembly macros

// High level and misc
#define hubset(h) asm volatile ("hubset %0" : : "ri"(h))
#define waitx(t) asm volatile ("waitx %0" : : "ri"(t))

#define setse1(x) asm volatile ("setse1 %0" : : "ri"(x))
#define setse2(x) asm volatile ("setse2 %0" : : "ri"(x))
#define setse3(x) asm volatile ("setse3 %0" : : "ri"(x))
#define setse4(x) asm volatile ("setse4 %0" : : "ri"(x))

#define waitse1() asm volatile ("waitse1" : :)
#define waitse2() asm volatile ("waitse2" : :)
#define waitse3() asm volatile ("waitse3" : :)
#define waitse4() asm volatile ("waitse4" : :)

#define wrc(x) asm volatile ("wrc %0" : "=r"(x) :)
#define wrnc(x) asm volatile ("wrnc %0" : "=r"(x) :)
#define wrz(x) asm volatile ("wrz %0" : "=r"(x) :)
#define wrnz(x) asm volatile ("wrnz %0" : "=r"(x) :)

// branching
#define cogret asm volatile ("ret")

// Pin control
#define dirh(pin) asm volatile ("dirh %0" : : "ri"(pin))
#define dirl(pin) asm volatile ("dirl %0" : : "ri"(pin))
#define outh(pin) asm volatile ("outh %0" : : "ri"(pin))
#define outl(pin) asm volatile ("outl %0" : : "ri"(pin))
#define outnot(pin) asm volatile ("outnot %0" : : "ri"(pin))
#define drvh(pin) asm volatile ("drvh %0" : : "ri"(pin))
#define drvl(pin) asm volatile ("drvl %0" : : "ri"(pin))
#define drvnot(pin) asm volatile ("drvnot %0" : : "ri"(pin))

// Test
#define testp(pin, res) asm volatile ("testp %1 wc\nwrc %0\n" : "=r"(res) : "ri"(pin))

// Smart pin control
#define rdpin(v, pin) asm volatile ("rdpin %0, %1" : "=r"(v) : "ri"(pin))
#define rqpin(v, pin) asm volatile ("rqpin %0, %1" : "=r"(v) : "ri"(pin))

#define wrpin(v, pin) asm volatile ("wrpin %0, %1" : : "ri"(v), "ri"(pin))
#define wxpin(v, pin) asm volatile ("wxpin %0, %1" : : "ri"(v), "ri"(pin))
#define wypin(v, pin) asm volatile ("wypin %0, %1" : : "ri"((int)v), "ri"((int)pin))

// Streamer
#define xinit(x, y) asm volatile ("xinit %0, %1" : : "ri"(x), "ri"(y))
#define setxfrq(x) asm volatile ("setxfrq %0" : : "ri"(x))
#define rdfast(x, y) asm volatile ("rdfast %0, %1" : : "ri"(x), "ri"(y))
#define wrfast(x, y) asm volatile ("wrfast %0, %1" : : "ri"(x), "ri"(y))

// LUT 
#define wrlut(x, addr) asm volatile ("wrlut %0, %1" : : "ri"(x), "ri"(addr))
#define rdlut(x, addr) asm volatile ("rdlut %0, %1" : "=r"(x), : "ri"(addr))

// Debugging
#define brk(x) asm volatile ("brk %0" : : "ri"(x))
#define cogbrk(x) asm volatile ("cogbrk %0" : : "ri"(x))
#define getbrk_wcz(x) asm volatile ("getbrk %0 wcz" : "=r"(x) :)
#define getbrk_wc(x) asm volatile ("getbrk %0 wc" : "=r"(x) :)
#define getbrk_wz(x) asm volatile ("getbrk %0 wz" : "=r"(x) :)

#define _clkfreq (*((int*)0x24))
#define _clkmode (*((int*)0x28))

#define _dbg_lock (*((int*)0x3c))

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

#define COG_PARAM_TO_OBJ(p, type) ((type*)p)

void __unreachable();

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
 * wait indefinitely with waitx to reduce power consumption
 */
#define busywait() while(1) waitx(CLKFREQ);

/**
 * lock/unlock the debug lock in case something else needs to access the debug UART
 */
#define __lock_dbg() _lock(_dbg_lock)
#define __unlock_dbg() _unlock(_dbg_lock)

/**
 * init the rtlib so that libcalls work (necessary for native cog functions)
 */
#define INIT_RTLIB  asm("setq2 #0x1ff\n augs #1\n rdlong $0, #0\n")
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
 * start a new cog dictated by mode. return if start was successful
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
void _uart_init(unsigned rx, unsigned tx, unsigned baud);

/**
 * write a character to the builtin UART
 */
void _uart_putc(char c, int p);

/**
 * check if there's a character in the buffer for pin p. 0 if there's no data, 1 if there is data
 */
int _uart_checkc(int p);

/**
 * read a character from the builtin UART. If no character is avilable, then return is undefined, so be sure to check
 * if anything's available wiht _uart_checkc first
 */
char _uart_getc(int p);

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
int _locktry(unsigned int l);

/**
 * try to lock a lock, blocking until it is acquired.
 */
void _lock(unsigned int l);

/**
 * release the lock
 */
void _unlock(unsigned int l);

#ifdef __cplusplus
}
#endif

#endif