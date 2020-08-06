/*
  Test a simple version of printf.

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

    volatile int i = 0x7fffffff-10;
    while(1) {
        printf("i as an int = %d, ", i);
        printf("i as an uint = %u, ", i);
        printf("i as a hex = %x\n", i);
        waitx(_CLOCKFREQ/10);
        i++;
    }

    while(1);
    return 0;
}