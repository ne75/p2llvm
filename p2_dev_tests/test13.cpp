/*
  test libcalls and more math

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"

#include "printf.h"

#define RX_PIN 63
#define TX_PIN 62

volatile int uart_clock_per_bits;

int main() {
    clkset(_SETFREQ, _CLOCKFREQ);
    uart_clock_per_bits = uart_init(RX_PIN, TX_PIN, 230400);

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
    return 0;
}