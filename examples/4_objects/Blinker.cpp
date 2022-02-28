#include "Blinker.h"

#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>

Blinker::Blinker(char pin, int delay) {
    this->pin = pin;
    this->delay = delay;
}

void Blinker::start() {
    const int stack_size = 32;
    unsigned int *stack = (unsigned int *)malloc(stack_size);
    int cogid = cogstart(blink, (int)this, (int *)stack, stack_size);
    printf("cog ID is %d\n", cogid);
}

void Blinker::blink(void *par) {
    Blinker *led = (Blinker*)par;

    dirh(led->pin);

    while(1) {
        outnot(led->pin);
        waitcnt(led->delay + CNT);
    }
}