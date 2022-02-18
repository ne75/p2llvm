#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "smartpins.h"
#include "streamer.h"

#define UART_RX_PIN 63
#define UART_TX_PIN 62
#define UART_BAUD 230400
#define DBG_UART_RX_PIN 03
#define DBG_UART_TX_PIN 02
#define DBG_UART_BAUD 3000000

// assembly macros

// Streamer
#define xinit(x, y) asm volatile ("xinit %0, %1" : : "r"(x), "r"(y))
#define setxfrq(x) asm volatile ("setxfrq %0" : : "r"(x))
#define rdfast(x, y) asm volatile ("rdfast %0, %1" : : "r"(x), "r"(y))
#define wrfast(x, y) asm volatile ("wrfast %0, %1" : : "r"(x), "r"(y))

// LUT 
#define wrlut(x, addr) asm volatile ("wrlut %0, %1" : : "r"(x), "r"(addr))
#define rdlut(x, addr) asm volatile ("rdlut %0, %1" : "=r"(x), : "r"(addr))

// Debugging
#define brk(x) asm volatile ("brk %0" : : "r"(x))
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
 * lock/unlock the debug lock in case something else needs to access the debug UART
 */
#define __lock_dbg() _lock(_dbg_lock)
#define __unlock_dbg() _unlock(_dbg_lock)

/**
 * init the rtlib so that libcalls work (necessary for native cog functions)
 */
#define INIT_RTLIB  asm("setq2 #0x1ff\n augs #1\n rdlong $0, #0\n")

// High level and misc
/**
 * @brief hubset set p2 processor
 * 
 * @param h 
 */
inline void _hubset(unsigned h) {
    asm("hubset %0" : : "r"(h));
}

/**
 * @brief wait t number of clocks
 * 
 * @param t 
 */
inline void _waitx(unsigned t) {
    asm("waitx %0" : : "r"(t));
}

/**
 * wait indefinitely with waitx to reduce power consumption
 */
inline void busywait() {
    while (1)
        _waitx(_clkfreq);
}

// Pin control
/**
 * @brief setup pin based on state
 * @param pin pin number to set
 * @param state 0 - set pin as input, 1 - set as output
 */
inline void _pinw(int pin, char state) {
    if (state)
        asm("dirh %0\n"::"r"(pin));
    else
        asm("dirl %0\n"::"r"(pin));
}

/**
 * @brief set pin direction to output
 * @param pin pin number to set
 */
inline void _dirh(int pin)
{
    asm volatile ("dirh %0\n"::"r"(pin));
}

/**
 * @brief set pin direction to input
 * @param pin pin number to set
 */
inline void _dirl(int pin)
{
    asm volatile ("dirl %0\n"::"r"(pin));
}

/**
 * @brief set pin state to high
 * @param pin pin number to set
 */
inline void _pinh(int pin)
{
    asm volatile ("drvh %0\n"::"r"(pin));
}

/**
 * @brief set pin state to low
 * @param pin pin number to set
 */
inline void _pinl(int pin)
{
    asm volatile ("drvl %0\n"::"r"(pin));
}

/**
 * @brief set pin state to random
 * @param pin pin number to set
 */
//void _pinrnd(char pin);

/**
 * @brief set pin to opposite state
 * @param pin pin number to set
 */
inline void _pinnot(int pin)
{
    asm volatile ("outnot %0\n"::"r"(pin));
}

/**
 * @brief read the logic state of I/O Pin
 * @param pin pin number to read
 */
inline int _pinr(int pin)
{
    int state;

    asm volatile ("testp %1 wc\n"
        "wrc %0\n"
        :"=r"(state):"r"(pin));
    return state;
}


/**
 * @brief test the logic state
 * @param pin pin to test
 */
inline int _testp(int pin) {
    int rslt;

    asm volatile ("testp %1 wc\n"
        "wrc %0\n"
        :"=r"(rslt)
        :"r"(pin));
    return rslt;
}

// Smart pin control
/**
 * @brief read smart pin state and clear in
 * @param pin smart pin number to read
 */
inline int _rdpin(int pin) {
    int rslt;

    asm ("rdpin %0, %1\n":"=r"(rslt):"r"(pin));
    return rslt;
}

/**
 * @brief read smart pin state don't clear in
 * @param pin pin number to read
 */
inline int _rqpin(int pin) {
    int rslt;

    asm ("rqpin %0, %1\n":"=r"(rslt):"r"(pin));
    return rslt;
}

/**
 * @brief set smart pin mode
 * @param pin smart pin to set
 * @param mode smart pin mode
 */
inline void _wrpin(int pin, unsigned mode) {
    asm ("wrpin %0, %1\n"::"r"(mode),"r"(pin));
}

/**
 * @brief write x value to smart pin
 * @param pin smart pin number
 * @param xval x value to set
 */
inline void _wxpin(int pin, unsigned xval) {
    asm ("wxpin %0, %1\n"::"r"(xval),"r"(pin));
}

/**
 * @brief write y value to smart pin
 * @param pin smart pin number
 * @param yval y value to set
 */
inline void _wypin(int pin, unsigned yval) {
    asm ("wypin %0, %1\n"::"r"(yval),"r"(pin));
}

/**
 * @brief float pin
 * @param pin pin number to float
 */
inline void _pinf(int pin) {
	asm ("fltl %0\n"::"r"(pin));
}

/**
 * @brief acknowledge smart pin
 * @param pin smart pin number
 */
//inline void _akpin(char pin) {
//    asm("akpin %0\n"::"r"(pin));
//}

/**
 * @brief configure smart
 * @param pin smart pin number to configure
 * @param mode smart pin mode value
 * @param xval x value to set
 * @param yval y value to set
 */
void _pinstart(int pin, unsigned mode, unsigned xval, unsigned yval);

/**
 * @brief clear smart pin
 * @param pin smart pin number
 */
void _pinclr(int pin);

/**
 * @brief set the P2 processor clock mode and frequency
 * 
 * @param clkmode _SETFREQ - macro of mode frequency
 * @param clkfreq _CLOCKFREQ - macro of freq
 * @details set P2_TARGET_MHZ 200 as first define in program which
 *          builds the two above macros for configuring the P2 clock 
 */
void _clkset(unsigned clkmode, unsigned clkfreq);

/**
 * @brief access the processor clock count
 * @return lower 32 bits of clock value
 */
inline unsigned int _cnt() {
    int x;
    asm("getct %0\n" : "=r"(x) : );
    return x;
}

/**
 * @brief wait unitil cnt
 * @param cnt clock counter to wait to
 */
inline void _waitcnt(unsigned int cnt) {
    asm("addct1 %0, #0" : : "r"(cnt));
    asm("waitct1");
}

/**
 * wait milliseconds
 */
void _wait(unsigned int milliseconds);

/**
 * wait milliseconds
 */
void _waitms(unsigned int milliseconds);

/**
 * waitus wait microseconds
 */
void _waitus(unsigned microseconds);

/**
 * get seconds since program started
 */
unsigned int _getsec(void);

/**
 * get milliseconds since program started
 */
unsigned int _getms(void);

/**
 * get microseconds since program started
 */
unsigned int _getus(void);

/**
 * @brief start a new cog based on mode
 * @param mode - 01 - new cog, 07 
 * @param f pointer to a function to start
 * @param par pointer to passed parameters
 * @return cog number started
 */
int _coginit(unsigned mode, void (*f)(void *), void *par) __attribute__((noinline));

/**
 * @brief start a new cog
 * 
 * @param f pointer to a function to start
 * @param par pointer to passed parameters
 * @param stack function stack pointer to use
 * @param stacksize stack pointer size
 * @return cog number started
 */
int cogstart(void (*f)(void *), void *par, unsigned *stack, int stacksize);

/**
 * @brief get current cog number
 * @return cog number
 */
inline int _cogid(void)
{
    int id;

    asm("cogid %0\n":"=r"(id):);
    return id;
}

/**
 * @brief stop a running cog
 * @param cog cog number to stop
 */
inline void _cogstop(int id)
{
    asm("cogstop %0\n"::"r"(id));
}

/**
 * reverse bits in x
 */
inline unsigned int _rev(unsigned int x) {
    asm("rev %0\n" :"+r"(x):);
    return x;
}

/**
 * @brief setup serial read and write for the given pins
 * @param rx recieve pin number to use
 * @param tx transmitt pin number to use
 * @param baud baud rate to use
 * @details This should be called after a clock change to adjust baud rate
 */
void _uart_init(unsigned rx, unsigned tx, unsigned baud);

/**
 * @brief send character
 * @param c character to send
 * @param p pin number setup to send
 */
void _uart_putc(char c, int p);

/**
 * @brief check if character is available
 * @param p pin number setup to receive data
 * @return status 0 if there's no character, 1 if there is character
 */
int _uart_checkc(int p);

/**
 * @brief receive a character from the serial pin
 * @param p pin number configured to recieve
 * @return character received
 * @details does not wait for a character and returns 0 if no character is available
 */
char _uart_getc(int p);

/**
 * @brief Get a lock from the lock table (16)
 * @return lock number
 */
inline unsigned int _locknew() {
    int x;
    asm("locknew %0" : "=r"(x) : );
    return x;
}

/**
 * @brief Return lock to the lock table (16)
 * @param l lock number
 */
inline void _lockret(unsigned int l) {
    asm("lockret %0" :: "r"(l));
}

/**
 * @brief try to get a lock
 * @param l lock number to get
 * @return 0 - lock not available 1 - got lock
 */
inline int _locktry(unsigned int l) {
    int x;
    asm("locktry %1 wc\n"
        "wrc %0\n"
        :"=r"(x) : "r"(l));
    return x;
}

/**
 * try to lock a lock, blocking until it is acquired.
 */
inline void _lock(unsigned int l) {
    asm(".L_locktry%=:\n"
        "locktry %0 wc\n"
        "if_nc jmp #.L_locktry%=" : : "r"(l));
}

/**
 * release the lock
 */
inline void _unlock(unsigned int l) {
    asm("lockrel %0" : : "r"(l));
}

#ifdef __cplusplus
}
#endif

#endif