/*
 * Test virtual functions in classes
 */
#define P2_TARGET_MHZ   200
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
    ~A();
    virtual void a() = 0;
};

class B : public A {
public:
    B() {};
    ~B() {};

    void a() override {
        printf("test a\n");
    }
};

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);

    B *a = new B();

    while(1) {
        a->a();
        waitx(CLKFREQ);
    }

    return 0;
}