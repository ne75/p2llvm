#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "smartpins.h"

#define hubset(h) asm("hubset %0" : : "r"(h));
#define waitx(t) asm("waitx %0" : : "r"(t))
#define dirh(pin) asm("dirh %0" : : "r"(pin))
#define dirl(pin) asm("dirl %0" : : "r"(pin))
#define outh(pin) asm("outh %0" : : "r"(pin))
#define outl(pin) asm("outl %0" : : "r"(pin))

#define wrpin(pin, r) asm("wrpin %0, %1" : : "r"(pin), "r"(r))
#define wxpin(pin, r) asm("wxpin %0, %1" : : "r"(pin), "r"(r))
#define wypin(pin, r) asm("wypin %0, %1" : : "r"(pin), "r"(r))

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
void _coginit(unsigned mode, void (*f)(void *), int par, unsigned *stack);

/*
 * start the next available cog in hub mode
 */
void _cognew(void (*f)(void *), int par, unsigned *stack);

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
 * eventually need to wrap this using the FILE interface in the C std library
 */
void _uart_putc(char c);

#ifdef __cplusplus
}
#endif

#endif