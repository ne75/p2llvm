#include "propeller2.h"

extern void __start();

unsigned int _cnt() {
    int x;
    asm volatile("getct %0" : "=r"(x));
    return x;
}

unsigned long long _cnt64() {
    unsigned low, high;
    // Rev B/C: GETCT WC captures the high half and latches the low half for
    // the following GETCT. Reading low first races a 32-bit counter rollover.
    asm volatile("getct %1 wc\ngetct %0" : "=r"(low), "=r"(high));
    return ((unsigned long long)high << 32) | low;
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
    hubset(clkmode & ~3);
    waitx(200000);
    hubset(clkmode | 3);
}

int _coginit(unsigned mode, void (*f)(void *), void *par) {
    int res;
    asm volatile("setq %2\n"
        "coginit %1, %3 wc\n"
        "wrc %0\n"
        : "=r"(res), "+r"(mode)
        : "r"(par), "r"(f)
        : "memory"
        );

    return !res ? mode : -1;
}

int cogstart(void (*f)(void *), int par, int *stack, unsigned int stacksize) {
    stack[0] = (int)f;
    stack[1] = par;
    return _coginit(0x10, __start, stack);
}

unsigned int _locknew() {
    int x;
    asm volatile("locknew %0 wc\nif_c neg %0, #1" : "=r"(x) : : "memory");
    return x;
}

void _lockret(unsigned int l) {
    asm volatile("lockret %0" : : "r"(l) : "memory");
}

void _lock(unsigned int l) {
    if (l > 15) return;
    asm volatile(".L_locktry%=:\n"
        "locktry %0 wc\n"
        "if_nc jmp #.L_locktry%=" : : "r"(l) : "memory");
}

int _locktry(unsigned int l) {
    int x;
    asm volatile("locktry %1 wc\nwrc %0" : "=r"(x) : "r"(l) : "memory");
    return x;
}

void _unlock(unsigned int l) {
    asm volatile("lockrel %0" : : "r"(l) : "memory");
}

void _uart_init(unsigned rx, unsigned tx, unsigned baud, unsigned mode) {
    dirl(rx);
    dirl(tx);

    // see async mode for explanation of these values
    unsigned int x = (_clkfreq/baud) << 16;
    
    x &= 0xfffffc00;
    x |= 7;

    int tx_mode = P_ASYNC_TX | P_TT_01;
    int rx_mode = P_ASYNC_RX;

    if (mode & UART_MODE_INVERT_TX) tx_mode |= P_INVERT_OUTPUT;
    if (mode & UART_MODE_INVERT_RX) rx_mode |= P_INVERT_A;

    wrpin(tx_mode, tx);
    wxpin(x, tx);
    dirh(tx);

    wrpin(rx_mode, rx);
    wxpin(x, rx);
    dirh(rx);
}

void _uart_putc(unsigned char c, int p) {
    if (p == DBG_UART_TX_PIN) {
        __lock_dbg();
    }
    wypin(c, p);
    waitx(20); // wait a little for pin to be set correctly
    int done;
    do {
        testp(p, done);
    } while(!done);

    if (p == DBG_UART_TX_PIN)
        __unlock_dbg();
}

int _uart_checkc(int p) {
    int have_data = 0;
    testp(p, have_data);
    return have_data;
}

unsigned char _uart_getc(int p) {
    unsigned x;
    rdpin(x, p);
    return (char)(x >> 24);
}

unsigned int crc32(unsigned char *s, int n) {
    unsigned int crc=0xFFFFFFFF;
    
    for (int i = 0; i < n; i++) {
        char ch = s[i];
        for (int j = 0; j < 8; j++) {
            unsigned int b = (ch^crc) & 1;
            crc >>= 1;
            if (b) crc = crc^0xEDB88320;
            ch >>=1;
        }
    }
    
    return ~crc;
}
