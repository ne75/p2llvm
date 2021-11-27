#include <stdio.h>
#include <stdint.h>
#include <debug.h>

#include <stdarg.h>

volatile uint64_t x = 0x2428149226142124;
volatile uint64_t y = 0x19da;

volatile int64_t a = -5000000000;
volatile int64_t b = 2;

int main() {
    printf("$\n"); // start of test character
    // brk(DEBUG_CODE_INIT);

    uint32_t t = CNT;
    uint64_t z = x/y; // should be 0x1660C174AE462
    t = CNT - t;
    printf("%llx\n", z);


    y = 1;
    z = x/y; // should be 0x2428149226142124
    printf("%llx\n", z);

    y = 0;
    z = x/y; // should be 0xffffffffffffffff
    printf("%llx\n", z);

    
    int64_t c = a/b;

    printf("%lld\n", c);

    a = 5000000000;
    b = -2;     
    c = a/b;

    printf("%lld\n", c);

    a = -5000000000;
    b = -2;
    t = CNT;
    c = a/b;
    t = CNT - t;
    
    printf("%lld\n", c);

    printf("~\n"); // end of test character

    busywait();
}