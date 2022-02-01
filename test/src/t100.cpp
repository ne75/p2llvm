/*
 * A propgcc-compatible (almost) blinker program. The main incompatibility is setting up
 * the clock, which will have to be P2 specific, but that's okay since most libraries won't
 * use it.
 *
 */

#define P2_TARGET_MHZ   200
#include <propeller.h>
#include <stdio.h>
#include <sys/p2es_clock.h>

int print(char *);
void output(char, char);

char Buffer[256];
char Data[] = "Hello World\n";


int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);

    dirh(56);
    dirh(57);
    outh(56);
    outl(57);

    //sprintf(Buffer, "Hello World\n");
    print(Data);

    //printf("Hello World %d\n", CLKFREQ);

    while(1) {
        outl(56);
        outnot(57);
        waitcnt(CLKFREQ + CNT);
    }
}

int print(char *buffer)
{
  int i = 0;

  while(buffer[i] != 0)
  {
     //_uart_putc(buffer[i++], 62);
     output(buffer[i++], 62);
  }
  return i;
}

void output(char c, char pin)
{
   int done;

   wypin(c, pin);
   waitx(20);
   do {
      testp(pin, done);
   } while (!done);
}
