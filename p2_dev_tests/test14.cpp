/*
  test C++ classes

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"
#include "printf.h"

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v,l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_copy(d,s)   __builtin_va_copy(d, s)

#define RX_PIN 63
#define TX_PIN 62

volatile int uart_clock_per_bits;

class Blinker {

    volatile unsigned int stack[32];

    char pin;
    int delay;

public:
    Blinker(char pin, int delay);

    void start();

    static void blink(void *par);
};

Blinker::Blinker(char pin, int delay) {
    this->pin = pin;
    this->delay = delay;
}

void Blinker::start() {
    cognew(blink, (int)this, (unsigned int *)stack);
}

void Blinker::blink(void *par) {
    Blinker *led = (Blinker*)par;

    dirh(led->pin);

    while(1) {
        outl(led->pin);
        waitx(led->delay);
        outh(led->pin);
        waitx(led->delay);
    }
}

Blinker led1(56, _CLOCKFREQ);
Blinker led2(57, _CLOCKFREQ/2);
Blinker led3(58, _CLOCKFREQ/3);
Blinker led4(59, _CLOCKFREQ/4);

void start_blinks(Blinker *led, ...) {
    va_list args;
    va_start(args, led);

    Blinker *l = led;
    while(l != 0) {
        l->start();
        l = va_arg(args, Blinker*);
    }
}

int main() {
    clkset(_SETFREQ, _CLOCKFREQ);
    uart_clock_per_bits = uart_init(RX_PIN, TX_PIN, 230400);

    printf("Variadic function test\n");

    start_blinks(&led1, &led2, &led3, &led4, 0);

    while(1) {
        printf("running blinking with c++!\n");
        waitx(_clkfreq);
    }
    return 0;
}