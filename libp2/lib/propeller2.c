#include <propeller2.h>

extern void __start();

unsigned int _cnt() {
    int x;
    asm("getct %0\n" : "=r"(x) : );
    return x;
}

void _waitcnt(unsigned int cnt) {
    // ADDCT1 D,#0; WAITCT1
    asm("addct1 %0, #0" : : "r"(cnt));
    asm("waitct1");
}

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
    int x = 0;
    asm("locktry %0 wc\n"
        "if_c mov %1, #1\n"
        :"=r"(x) : "r"(l));
    return x;
}

void _unlock(unsigned int l) {
    asm("lockrel %0" : : "r"(l));
}

void _hubset(unsigned h) {
    asm("hubset %0" : : "r"(h));
}

void _waitx(unsigned t) {
    asm("waitx %0" : : "r"(t));
}

void _pinw(char pin, char state) {
    if (state)
        asm("dirh %0\n"::"r"(pin));
    else
        asm("dirl %0\n"::"r"(pin));
}

void _dirh(char pin) {
    asm("dirh %0\n"::"r"(pin));
}

void _dirl(char pin) {
    asm("dirl %0\n"::"r"(pin));
}
void _pinh(char pin) {
    asm("outh %0\n"::"r"(pin));
}

void _pinl(char pin) {
    asm("outl %0\n"::"r"(pin));
}

//void _pinrnd(char pin) {
//    asm("outrnd %0\n": :"r"(pin));
//}

void _pinnot(char pin) {
    asm("outnot %0\n"::"r"(pin));
}

int _pinr(char pin) {
    int state = 0;

    asm("testp %1 wc\n"
        "if_c  mov %0, #1\n"
        :"+r"(state)
        :"r"(pin));
    return state;
}

int _testp(char pin) {
    int rslt = 0;

    asm("testp %1 wc\n"
        "if_c mov %0, #1\n"
        :"+r"(rslt)
        :"r"(pin));
    return rslt;
}

int _rdpin(char pin) {
    int rslt = 0;

    asm("rdpin %0, %1\n":"=r"(rslt):"r"(pin));
    return rslt;
}

int _rqpin(char pin) {
    int rslt = 0;

    asm("rqpin %0, %1\n":"=r"(rslt):"r"(pin));
    return rslt;
}

void _wrpin(char pin, char state) {
    asm("wrpin %0, %1\n"::"r"(state),"r"(pin));
}

void _wxpin(char pin, char state) {
    asm("wxpin %0, %1\n"::"r"(state),"r"(pin));
}

void _wypin(char pin, char state) {
    asm("wypin %0, %1\n"::"r"(state),"r"(pin));
}

//void _akpin(char pin) {
//    asm("akpin %0\n"::"r"(pin));
//}

void _pinstart(char pin, unsigned mode, unsigned xval, unsigned yval) {
    asm("dirl %0\n"
        "wrpin %1, %0\n"
        "wxpin %2, %0\n"
        "wypin %3, %0\n"
        "dirh %0\n"
        ::"r"(pin), "r"(mode), "r"(xval), "r"(yval));
}

unsigned int _rev(unsigned int x) {
    asm("rev %0" : : "r"(x));
    return x;
}

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
        _wait(1);
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

    return (char)(x >> 24);
}