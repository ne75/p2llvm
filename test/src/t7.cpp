#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define RX_PIN 63
#define TX_PIN 62

class A {
public:
    A() {};
    ~A() {};
    virtual void a() = 0;
};

class B : public A {
public:
    B() {};
    ~B() {};

    void a() override {
        printf("virtual a()\n");
    }
};

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    B b;

    b.a();

    printf("~\n"); // end of test character
    return 0;
}