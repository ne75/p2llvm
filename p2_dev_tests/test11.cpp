/*
  Test switch statements

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"

#define RX_PIN 63
#define TX_PIN 62

volatile int uart_clock_per_bits;

void uart_str(const char *str) {
    while (*str) {
        uart_putc(*str);
        str++;
        waitx(uart_clock_per_bits*10); // wait for the bits to send
    }
}

void switch_test(int i) {
    switch (i) {
        case 0:
            uart_str("case 0\n");
            break;
        case 1:
            uart_str("case 1\n");
            break;
        case 2:
            uart_str("case 2 with fallthrough to ");
        case 3:
            uart_str("case 3\n");
            break;
        case 4:
            uart_str("case 4\n");
            break;
        default:
            uart_str("unknown case\n");
            return;
    }
}

int main() {
    clkset(_SETFREQ, _CLOCKFREQ);
    uart_clock_per_bits = uart_init(RX_PIN, TX_PIN, 230400);

    uart_str("Switch statement test\n");

    volatile int i = 0;
    switch_test(i);
    waitx(_CLOCKFREQ);

    i = 2;
    switch_test(i);
    waitx(_CLOCKFREQ);

    i = 5;
    switch_test(i);

    i = -1;
    if (i >= 0) {
        uart_str("i >= 0\n");
    }

    while(1);
    return 0;
}