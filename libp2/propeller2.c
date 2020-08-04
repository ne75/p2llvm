#include "propeller2.h"

extern void _start();

unsigned _uart_tx_pin;
unsigned _uart_rx_pin;
unsigned _uart_clock_per_bit;

unsigned uart_init(unsigned rx, unsigned tx, unsigned baud) {
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

void uart_putc(char c) {
    wypin(c, _uart_tx_pin);
    waitx(_uart_clock_per_bit*10); // eventually should be a wait
}

void clkset(unsigned clkmode, unsigned clkfreq) {
    // changing the clock mode is a bit complex because of how the pll works and it needs to settle
    hubset(0xf0);
    _clkmode = clkmode;
    _clkfreq = clkfreq;
    hubset(clkmode);
    waitx(200000);
    hubset(clkmode | 3);
}

void coginit(unsigned mode, void (*f)(void *), int par, unsigned *stack) {
    stack[0] = (int)f;
    stack[1] = par;
    asm("setq %0\n"
        "coginit %1, %2"
        : // no outputs
        : "r"(stack), "r"(mode), "r"(_start)
        );
}

void cognew(void (*f)(void *), int par, unsigned *stack) {
    coginit(0x10, f, par, stack);
}