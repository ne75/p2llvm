#ifndef _PROPELLER2_H
#define _PROPELLER2_H

#include "pins.h"
#include "streamer.h"

#define DBG_UART_RX_PIN 63
#define DBG_UART_TX_PIN 62
#define DBG_UART_BAUD 2000000

#define UART_MODE_INVERT_RX 1
#define UART_MODE_INVERT_TX 2

#define SPI_FLASH_MISO  58
#define SPI_FLASH_MOSI  59
#define SPI_FLASH_CLK   60
#define SPI_FLASH_CS    61

#define E_IN_RISE   (1 << 6)
#define E_IN_FALL   (2 << 6)
#define E_IN_CHANGE (3 << 6)
#define E_IN_LOW    (4 << 6)
#define E_IN_HIGH   (6 << 6)

// assembly macros

// High level and misc
#define hubset(h) asm volatile ("hubset %0" : : "ri"((int)h))
#define waitx(t) asm volatile ("waitx %0" : : "ri"((int)t))

#define addct1(t, dt) asm volatile ("addct1 %0, %1" : "+r"(t): "ri"(dt))
#define addct2(t, dt) asm volatile ("addct2 %0, %1" : "+r"(t): "ri"(dt))
#define addct3(t, dt) asm volatile ("addct3 %0, %1" : "+r"(t): "ri"(dt))

#define setse1(x) asm volatile ("setse1 %0" : : "ri"((int)x))
#define setse2(x) asm volatile ("setse2 %0" : : "ri"((int)x))
#define setse3(x) asm volatile ("setse3 %0" : : "ri"((int)x))
#define setse4(x) asm volatile ("setse4 %0" : : "ri"((int)x))

#define waitct1() asm volatile ("waitct1")
#define waitct2() asm volatile ("waitct2")
#define waitct3() asm volatile ("waitct3")

#define waitse1() asm volatile ("waitse1")
#define waitse2() asm volatile ("waitse2")
#define waitse3() asm volatile ("waitse3")
#define waitse4() asm volatile ("waitse4")

#define pollint() ({ int r = 0; asm volatile ("pollint wc\nwrc %0" : "=r"(r)); r; })
#define pollct1() ({ int r = 0; asm volatile ("pollct1 wc\nwrc %0" : "=r"(r)); r; })
#define pollct2() ({ int r = 0; asm volatile ("pollct2 wc\nwrc %0" : "=r"(r)); r; })
#define pollct3() ({ int r = 0; asm volatile ("pollct3 wc\nwrc %0" : "=r"(r)); r; })
#define pollse1() ({ int r = 0; asm volatile ("pollse1 wc\nwrc %0" : "=r"(r)); r; })
#define pollse2() ({ int r = 0; asm volatile ("pollse2 wc\nwrc %0" : "=r"(r)); r; })
#define pollse3() ({ int r = 0; asm volatile ("pollse3 wc\nwrc %0" : "=r"(r)); r; })
#define pollse4() ({ int r = 0; asm volatile ("pollse4 wc\nwrc %0" : "=r"(r)); r; })
#define pollpat() ({ int r = 0; asm volatile ("pollpat wc\nwrc %0" : "=r"(r)); r; })
#define pollfbw() ({ int r = 0; asm volatile ("pollfbw wc\nwrc %0" : "=r"(r)); r; })
#define pollxmt() ({ int r = 0; asm volatile ("pollxmt wc\nwrc %0" : "=r"(r)); r; })
#define pollxfi() ({ int r = 0; asm volatile ("pollxfi wc\nwrc %0" : "=r"(r)); r; })
#define pollxro() ({ int r = 0; asm volatile ("pollxro wc\nwrc %0" : "=r"(r)); r; })
#define pollxrl() ({ int r = 0; asm volatile ("pollxrl wc\nwrc %0" : "=r"(r)); r; })
#define pollatn() ({ int r = 0; asm volatile ("pollatn wc\nwrc %0" : "=r"(r)); r; })
#define pollqmt() ({ int r = 0; asm volatile ("pollqmt wc\nwrc %0" : "=r"(r)); r; })

#define wrc(x) asm volatile ("wrc %0" : "=r"(x))
#define wrnc(x) asm volatile ("wrnc %0" : "=r"(x))
#define wrz(x) asm volatile ("wrz %0" : "=r"(x))
#define wrnz(x) asm volatile ("wrnz %0" : "=r"(x))

#define cogid(x) asm volatile("cogid %0" : "=r"(x))
#define cogstop(x) asm volatile("cogstop %0" :: "ri"(x))

// branching
#define cogret asm volatile ("ret")

// Pin control
#define dirh(pin) asm volatile ("dirh %0" : : "ri"((int)pin))
#define dirl(pin) asm volatile ("dirl %0" : : "ri"((int)pin))
#define outh(pin) asm volatile ("outh %0" : : "ri"((int)pin))
#define outl(pin) asm volatile ("outl %0" : : "ri"((int)pin))
#define outnot(pin) asm volatile ("outnot %0" : : "ri"((int)pin))
#define drvh(pin) asm volatile ("drvh %0" : : "ri"((int)pin))
#define drvl(pin) asm volatile ("drvl %0" : : "ri"((int)pin))
#define drvnot(pin) asm volatile ("drvnot %0" : : "ri"((int)pin))

// Test
#define testp(pin, res) asm volatile ("testp %1 wc\nwrc %0\n" : "=r"(res) : "ri"((int)pin))

// CORDIC
#define qcos(x, angle, r) asm volatile ("qrotate %[amp], %[ang]\ngetqx %[res]" : [res]"=r"(r) : [amp]"ri"(x), [ang]"ri"(angle))
#define qsin(x, angle, r) asm volatile ("qrotate %[amp], %[ang]\ngetqy %[res]" : [res]"=r"(r) : [amp]"ri"(x), [ang]"ri"(angle))
#define qrotate(x, y, angle) asm volatile ("setq %0\nqrotate %1, %2\n" : : "ri"(y), "ri"(x), "ri"(angle))
#define qvector(x, y) asm volatile ("qvector %0, %1\n" : : "ri"(x), "ri"(y))
#define qdiv(x, y) asm volatile ("qdiv %0, %1" :: "ri"(x), "ri"(y))
#define qfrac(x, y) asm volatile ("qfrac %0, %1" :: "ri"(x), "ri"(y))
#define qlog(x) asm volatile ("qlog %0" :: "ri"(x))
#define qexp(x) asm volatile ("qexp %0" :: "ri"(x))
#define getqx(x) asm volatile ("getqx %0" : "=r"(x))
#define getqy(y) asm volatile ("getqy %0" : "=r"(y))

// Smart pin control
#define rdpin(v, pin) asm volatile ("rdpin %0, %1" : "=r"(v) : "ri"((int)pin))
#define rqpin(v, pin) asm volatile ("rqpin %0, %1" : "=r"(v) : "ri"((int)pin))

#define wrpin(v, pin) asm volatile ("wrpin %0, %1" : : "ri"((int)v), "ri"((int)pin))
#define wxpin(v, pin) asm volatile ("wxpin %0, %1" : : "ri"((int)v), "ri"((int)pin))
#define wypin(v, pin) asm volatile ("wypin %0, %1" : : "ri"((int)v), "ri"((int)pin))

#define akpin(pin) asm volatile ("akpin %0" : : "ri"((int)pin))

// Streamer
#define xinit(x, y) asm volatile ("xinit %0, %1" : : "ri"((int)x), "ri"((int)y))
#define setxfrq(x) asm volatile ("setxfrq %0" : : "ri"((int)x))
#define rdfast(x, y) asm volatile ("rdfast %0, %1" : : "ri"((int)x), "ri"((int)y))
#define wrfast(x, y) asm volatile ("wrfast %0, %1" : : "ri"((int)x), "ri"((int)y))

// LUT 
#define wrlut(x, addr) asm volatile ("wrlut %0, %1" : : "ri"((int)x), "ri"((int)addr))
#define rdlut(x, addr) asm volatile ("rdlut %0, %1" : "=r"(x), : "ri"((int)addr))

// Debugging
#define brk(x) asm volatile ("brk %0" : : "ri"((int)x))
#define cogbrk(x) asm volatile ("cogbrk %0" : : "ri"((int)x))
#define getbrk_wcz(x) asm volatile ("getbrk %0 wcz" : "=r"(x))
#define getbrk_wc(x) asm volatile ("getbrk %0 wc" : "=r"(x))
#define getbrk_wz(x) asm volatile ("getbrk %0 wz" : "=r"(x))

// Misc math
#define zerox(x, n) asm volatile ("zerox %0, %1" : "+r"(x) : "ri"(n))

#define _clkfreq (*((int*)0x14))
#define _clkmode (*((int*)0x18))
#define _dbgbaud (*((int*)0x1c))

#define _dbg_lock (*((int*)0x3c))

#define reboot() hubset(0x10000000)

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

#if defined(__cplusplus)
//
// is_lvalue_reference<T>
//
template<typename T> struct is_lvalue_reference      { static constexpr bool value = false; };
template<typename T> struct is_lvalue_reference<T&>  { static constexpr bool value = true; };


//
// remove_reference<T>
//
template<typename T> struct remove_reference      { using type = T; };
template<typename T> struct remove_reference<T&>  { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };

template<typename T>using remove_reference_t = typename remove_reference<T>::type;

//
// is_same<T, U>
//
template<typename T, typename U> struct is_same {
    static constexpr bool value = false;
};

template<typename T> struct is_same<T, T> {
    static constexpr bool value = true;
};

//
// move (like std::move)
//
template<typename T> constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

//
// forward (like std::forward)
//
template<typename T> constexpr T&& forward(remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T> constexpr T&& forward(remove_reference_t<T>&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "bad forward of rvalue as lvalue");
    return static_cast<T&&>(t);
}

inline void* operator new(unsigned long l, void* ptr) {
  return ptr;
}

inline void operator delete(void*, void*) {}
#endif

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

#ifndef NULL
#define NULL (0)
#endif

#ifndef _SIZE_T
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
  typedef __SIZE_TYPE__ size_t;
#define _SIZE_T
#endif

void *memset(void *dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);

/**
 * run clock configuration to the desired clock mode and clock frequency
 */
void _clkset(unsigned clkmode, unsigned clkfreq);

/**
 * return the current count
 */
unsigned int _cnt();

/**
 * return the current 64 bit count
 */
unsigned long long _cnt64();

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
static inline unsigned int _rev(unsigned int x) {
    asm volatile ("rev %0" : "+r"(x));
    return x;
}

/**
 * get the number of ones in x
 */
static inline unsigned int _ones(unsigned int x) {
    int ones = 0;
    asm("ones %0, %1" : "=r"(ones) : "r"(x));
    return ones;
}

/**
 * return the parity of the given number. 
 * @returns 1 if there's an odd number of ones, 0 if even
 */
static inline int _parity(unsigned int x) {
    int par = 0;
    asm("ones %[x], %[x] wc\n"
        "wrc %[par]"
         : [x]"+r"(x), [par]"=r"(par));

    return par;
}

/**
 * get the position of the top-most 1 in x. If there are none, -1 is returned
 */
static inline int _encod(unsigned int x) {
    int top;
    asm volatile(
        "encod %0, %1 wc\n"
        "if_nc mov %0, #-1" : "=r"(top) : "r"(x)
    );

    return top;
}

/**
 * initialize the given rx/tx pins in async mode (uart)
 * this needs to be called again if using UART and clkset is called to adjust baud rates
 */
void _uart_init(unsigned rx, unsigned tx, unsigned baud, unsigned mode);

/**
 * write a character to the builtin UART
 */
void _uart_putc(unsigned char c, int p);

/**
 * check if there's a character in the buffer for pin p. 0 if there's no data, 1 if there is data
 */
int _uart_checkc(int p);

/**
 * read a character from the builtin UART. If no character is avilable, then return is undefined, so be sure to check
 * if anything's available wiht _uart_checkc first
 */
unsigned char _uart_getc(int p);

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

/**
 * compute a crc32
 * 
 * TODO: use propeller's CRC instructions
 */
unsigned int crc32(unsigned char *s, int n);

// Misc useful macros

#define MIN2(a,b) (a < b ? a:b)
#define MIN3(a,b,c) (MIN2(MIN2(a,b),c))

#ifdef __cplusplus
}
#endif

#endif