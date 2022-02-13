#include <propeller2.h>

extern void __start();

extern unsigned int _cnt();

extern void busywait();

extern void _waitcnt(unsigned int cnt);

void _wait(unsigned milliseconds)
{
    // do seconds if any
    while (milliseconds > 1000) {
       _waitx(_clkfreq);
       milliseconds -= 1000;
    }
    if (milliseconds > 0) {
       milliseconds = _clkfreq / 1000 * milliseconds;
       _waitx(milliseconds);
    }
}

#define _waitms(milliseconds) _wait(milliseconds)

void _waitus(unsigned microseconds)
{
    microseconds = _clkfreq / 1000000 * microseconds;
    _waitx(microseconds);
}

unsigned int _getsec()
{
    unsigned int upper, lower;

    asm("getct %0 wc\n"
        "getct %1\n"
        "setq %0\n"
        "qdiv %1, %2\n"
        "getqx %1\n"
        :"+r"(upper), "+r"(lower)
        :"r"(_clkfreq));
        return lower;
}

unsigned int _getms()
{
    unsigned int upper, lower;
    unsigned int msec = _clkfreq / 1000;

    asm("getct %0 wc\n"
        "getct %1\n"
        "setq %0\n"
        "qdiv %1, %2\n"
        "getqx %1\n"
        :"+r"(upper), "+r"(lower)
        :"r"(msec));
        return lower;
}

unsigned int _getus()
{
    unsigned int upper, lower;
    unsigned int msec = _clkfreq / 1000000;

    asm("getct %0 wc\n"
        "getct %1\n"
        "setq %0\n"
        "qdiv %1, %2\n"
        "getqx %1\n"
        :"+r"(upper), "+r"(lower)
        :"r"(msec));
        return lower;
}

void _clkset(unsigned clkmode, unsigned clkfreq) {
    // changing the clock mode is a bit complex because of how the pll works and it needs to settle
    _hubset(0xf0);
    _clkmode = clkmode;
    _clkfreq = clkfreq;
    _hubset(clkmode);
    _waitx(200000);
    _hubset(clkmode | 3);
}

int _coginit(unsigned mode, void (*f)(void *), void *par) {
    int res = 0;
    asm("setq %1\n"
        "coginit %2, %3 wc\n"
        "if_nc mov %0, #1\n"
        : "=r"(res)
        : "r"(par), "r"(mode), "r"(f)
        );

    return res;
}

int cogstart(void (*f)(void *), void *par, unsigned *stack, int stacksize) {
    int started;
    int mode;
    stack[0] = (int)f;
    stack[1] = (int)par;

    mode = 0x10;

    asm("setq %2\n"
        "coginit %1, %3 wc\n"
        "if_nc mov %0, %1\n"
        : "=r"(started), "+r"(mode)
        : "r"(stack), "r"(__start));

    //return _coginit(0x10, __start, stack;
    return started;
}

extern unsigned int _locknew();

extern void _lockret(unsigned int l);

extern void _lock(unsigned int l);

extern int _locktry(unsigned int l);

extern void _unlock(unsigned int l);

extern void _hubset(unsigned h);

extern void _waitx(unsigned t);

extern void _pinw(char pin, char state);

extern void _dirh(char pin);

extern void _dirl(char pin);

extern void _pinh(char pin);

extern void _pinl(char pin);

extern void _pinnot(char pin);

extern int _pinr(char pin);

extern int _testp(char pin);

extern int _rdpin(char pin);

extern int _rqpin(char pin);

extern void _pinf(char pin);

extern void _wrpin(char pin, unsigned mode);

extern void _wxpin(char pin, unsigned xval);

extern void _wypin(char pin, unsigned yval);

//extern void _akpin(char pin);

void _pinstart(char pin, unsigned mode, unsigned xval, unsigned yval) {
    asm("dirl %0\n"
        "wrpin %1, %0\n"
        "wxpin %2, %0\n"
        "wypin %3, %0\n"
        "dirh %0\n"
        ::"r"(pin), "r"(mode), "r"(xval), "r"(yval));
}

void _pinclr(char pin)
{
    asm("dirl %0\n"
        "wrpin %0, #0\n"
        ::"r"(pin));
}

extern unsigned int _rev(unsigned int x);

void _uart_init(unsigned rx, unsigned tx, unsigned baud) {

    // see async mode for explanation of these values
    unsigned int x = _clkfreq/baud;

    x <<= 16;
    x &= 0xfffffc00;
    x |= 7;

    _pinstart(tx, P_OE | P_ASYNC_TX, x, 0);

    _pinstart(rx, P_ASYNC_RX, x, 0);
}

void _uart_putc(char c, int p) {
    int done = 0;

    if (p == DBG_UART_TX_PIN) {
        __lock_dbg();
    }
    _wypin(p, c);
    _waitx(20); // wait a little for pin to be set correctly
    for (int i=0;i<100;i++)
    {
        done = _testp(p);
        if (done != 0)
            break;
        _waitus(50);
    }

    __unlock_dbg();
}

int _uart_checkc(int p) {
    int have_data = 0;

    have_data = _testp(p);

    return have_data;
}

char _uart_getc(int p) {
    unsigned x;

    x = _rdpin(p);
    x = x >> 24;
    
    return (char)x;
}