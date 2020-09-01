/*
  test libc I/O drivers.

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller.h"
#include "sys/p2es_clock.h"
//#include "printf.h"

#include <stdio.h>
#include <stdarg.h>

#define RX_PIN 63
#define TX_PIN 62

// _Driver *_driverlist[] = {
//     &_SimpleSerialDriver,
//     &_FullDuplexSerialDriver,
//     NULL
// };

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);

    dirh(56);

    while(1) {
        char c = getchar();
        printf("%c", c);
    }

    return 0;
}