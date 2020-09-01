#include "propeller2.h"

extern void __start();

unsigned _uart_tx_pin;
unsigned _uart_rx_pin;
unsigned _uart_clock_per_bit;

unsigned int _cnt() {
    int x;
    asm("getct $r31" : "=r"(x) : );
    return x;
}

void _waitcnt(unsigned int cnt) {
    // ADDCT1 D,#0; WAITCT1
    asm("addct1 %0, #0" : : "r"(cnt));
    asm("waitct1");
}

void _clkset(unsigned clkmode, unsigned clkfreq) {
    // changing the clock mode is a bit complex because of how the pll works and it needs to settle
    hubset(0xf0);
    _clkmode = clkmode;
    _clkfreq = clkfreq;
    hubset(clkmode);
    waitx(200000);
    hubset(clkmode | 3);
}

int _coginit(unsigned mode, void (*f)(void *), void *par) {
    asm("setq %0\n"
        "coginit %1, %2 wc"
        : // no outputs
        : "r"(par), "r"(mode), "r"(f)
        );
    int res;
    wrc(res);

    if (res) {
        return -1;
    } else {
        return mode;
    }
}

int cogstart(void (*f)(void *), int par, int *stack, unsigned int stacksize) {
    stack[0] = (int)f;
    stack[1] = par;
    return _coginit(0x10, __start, stack);
}

unsigned int _locknew() {
    int x;
    asm("locknew %0" : "=r"(x) : );
    return x;
}

void _lockret(unsigned int l) {
    asm("locknew %0" : : "r"(l));
}

void _lock(atomic_t l) {
    asm("locktry %0 wc" : : "r"(l));
    asm("if_nc jmp #-8");
}

void _unlock(atomic_t l) {
    asm("lockrel %0" : : "r"(l));
}

unsigned int _rev(unsigned int x) {
    asm("rev %0" : : "r"(x));
    return x;
}

unsigned _uart_init(unsigned rx, unsigned tx, unsigned baud) {
    _uart_rx_pin = rx;
    _uart_tx_pin = tx;
    dirl(rx);
    dirl(tx);

    // see async mode for explanation of these values
    _baudrate = baud;
    unsigned int x = _clkfreq/baud;
    _uart_clock_per_bit = x;

    x <<= 16;
    x &= 0xfffffc00;
    x |= 7;

    wrpin(ASYNC_TX_MODE, tx);
    wxpin(x, tx);
    dirh(tx);

    wrpin(ASYNC_RX_MODE, rx);
    wxpin(x, rx);
    dirh(rx);

    return _uart_clock_per_bit;
}

void _uart_putc(char c) {
    wypin(c, _uart_tx_pin);
    waitx(_uart_clock_per_bit*10); // eventually should be a wait based on the buffer
}

char _uart_getc() {
    int has_dat = 0;

    testp(_uart_rx_pin, has_dat);

    if (has_dat) {
        int x;
        rdpin(x, _uart_rx_pin);
        return (char)(x >> 24);
    } else {
        return -1;
    }
}