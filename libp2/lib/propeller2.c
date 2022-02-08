#include "propeller2.h"

extern void __start();

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
    int res;
    asm("setq %1\n"
        "coginit %2, %3 wc\n"
        "wrc %0\n"
        : "=r"(res)
        : "r"(par), "r"(mode), "r"(f)
        );

    return !res;
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
    asm("lockret %0" : : "r"(l));
}

void _lock(unsigned int l) {
    asm(".L_locktry%=:\n"
        "locktry %0 wc\n"
        "if_nc jmp #.L_locktry%=" : : "r"(l));
}

int _locktry(unsigned int l) {
    int x;
    asm("locktry %0 wc" : : "r"(l));
    wrc(x);
    return x;
}

void _unlock(unsigned int l) {
    asm("lockrel %0" : : "r"(l));
}

unsigned int _rev(unsigned int x) {
    asm("rev %0" : : "r"(x));
    return x;
}

void _uart_init(unsigned rx, unsigned tx, unsigned baud) {
    dirl(rx);
    dirl(tx);

    // see async mode for explanation of these values
    unsigned int x = _clkfreq/baud;

    x <<= 16;
    x &= 0xfffffc00;
    x |= 7;

    wrpin(P_ASYNC_TX | P_TT_01, tx);
    wxpin(x, tx);
    dirh(tx);

    wrpin(P_ASYNC_RX, rx);
    wxpin(x, rx);
    dirh(rx);
}

void _uart_putc(char c, int p) {
    if (p == DBG_UART_TX_PIN) {
        __lock_dbg();
    }
    wypin(c, p);
    waitx(20); // wait a little for pin to be set correctly
    int done;
    do {
        testp(p, done);
    } while(!done);

    __unlock_dbg();
}

int _uart_checkc(int p) {
    int have_data = 0;
    testp(p, have_data);
    if (have_data) return 1;
    
    return 0;
}

char _uart_getc(int p) {
    unsigned x;
    rdpin(x, p);
    return (char)(x >> 24);
}