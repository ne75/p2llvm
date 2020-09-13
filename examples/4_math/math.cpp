/*
 * A basic test of using libcalls (for math functions). Eventually expand this example to include floats
 * there's no float hardware so it will take a decent amount of work to implement.
 */
#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400); // FIXME: this is SUPPOSED to called by the C standard lib but it isn't working

    volatile int i = 10;
    volatile int j = 5;
    printf("test 1\n");
    printf("%d/%d = %d\n", i, j, i/j);

    i = -10;
    j = 5;
    printf("test 2\n");
    printf("%d/%d = %d\n", i, j, i/j);

    i = 10;
    j = -5;
    printf("test 3\n");
    printf("%d/%d = %d\n", i, j, i/j);

    i = -10;
    j = -5;
    printf("test 4\n");
    printf("%d/%d = %d\n", i, j, i/j);

    i = -10;
    j = 5;
    printf("test 5\n");
    printf("%d*%d = %d\n", i, j, i*j);

    i = -10;
    j = -5;
    printf("test 6\n");
    printf("%d*%d = %d\n", i, j, i*j);

    while(1);

    while(1);
}