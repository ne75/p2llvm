#define P2_TARGET_MHZ   200
#include <propeller.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/p2es_clock.h>

unsigned char b[128];
unsigned char Buffer[128];
unsigned char data[] = "Now is the time for all good men.";


int main(int argc, char** argv) {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);
    
    printf("Starting...\n");

    //b = malloc(128);

    printf("Coping into buffer\n");

    memcpy(Buffer, data, 24);

    printf("Coping Buffer to memory\n");

    memcpy(b, Buffer, 24);

    printf("Memory: %24.24s\n", b);

    printf("Done\n");

    while (1)
    {
        waitx(1000);
    }
}