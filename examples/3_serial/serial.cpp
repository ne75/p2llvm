/*
 * A basic test of using the built-in UART interface, implemented using smart pins. It uses the file structure internal to
 * the C std lib to write to the serial devices.
 */
#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400); // FIXME: this is SUPPOSED to called by the C standard lib but it isn't working

    printf("Hello World!\n");

    while(1);
}