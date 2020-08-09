/*
  test libp2

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller.h"
#include "sys/p2es_clock.h"

#include "printf.h"

#include <sys/va_list.h>
#include <stdarg.h>

#define RX_PIN 63
#define TX_PIN 62

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
    _cognew(blink, (int)this, (unsigned int *)stack);
}

void Blinker::blink(void *par) {
    Blinker *led = (Blinker*)par;

    int pin = led->pin;

    DIRB |= 1 << (led->pin-32);

    while(1) {
        OUTB ^= 1 << (led->pin-32);
        waitcnt(led->delay + CNT);
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
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);

    printf("Variadic function test\n");

    start_blinks(&led1, &led2, &led3, &led4, 0);

    while(1) {
        printf("running blinking with c++!\n");
        waitx(CLKFREQ);
    }
    return 0;
}