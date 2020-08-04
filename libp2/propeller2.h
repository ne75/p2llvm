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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * run clock configuration to the desired clock mode and clock frequency
 */
void clkset(unsigned clkmode, unsigned clkfreq);

/*
 * start a new hub mode cog dictated by mode
 */
void coginit(unsigned mode, void (*f)(void *), int par, unsigned *stack);

/*
 * start the next available cog in hub mode
 */
void cognew(void (*f)(void *), int par, unsigned *stack);

/*
 * initialize the given rx/tx pins in async mode (uart)
 * this needs to be called again if using UART and clkset is called to adjust baud rates
 */
unsigned uart_init(unsigned rx, unsigned tx, unsigned baud);

/*
 * write a character to the builtin UART
 * eventually need to wrap this using the FILE interface in the C std library
 */
void uart_putc(char c);

#ifdef __cplusplus
}
#endif

#endif