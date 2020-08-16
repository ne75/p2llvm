/* pins for the bit-banging */
/* pick pins that are in the range 0-31 and are not being used */
#define BASEPIN 4
#define SPI_CS   (BASEPIN)
#define SPI_MOSI (BASEPIN+1)
#define SPI_MISO (BASEPIN+2)
#define SPI_CK   (BASEPIN+3)

#if defined(P2GCC)
/* p2gcc doesn't have propeller2.h */
#include <propeller.h>
#include <stdio.h>

#if 1
#define setoutput(pin) DIRA |= (1<<(pin))
#define sethi(pin)     OUTA |= (1<<(pin))
#define setlo(pin)     OUTA &= ~(1<<(pin))
#else
// p2gcc doesn't support pinl/pinh
// also, oddly enough, the direct register accesses above are faster than using
// these instructions, probably because for the latter GCC can convert the
// branches in the if statement into conditionalized instructions
#define setoutput(pin) __asm__ volatile ("dirh %0" : : "r" (pin) : )
#define sethi(pin)     __asm__ volatile ("outh %0" : : "r" (pin) : )
#define setlo(pin)     __asm__ volatile ("outl %0" : : "r" (pin) : )
#endif
#define getcnt(x)      (x = CNT)

#elif defined(__p2llvm__)

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"
#include "printf.h"
// don't try to include stdio.h for LLVM

// LLVM doesn't have pinl/pinh yet, but it does have dirh/outh/outl
#define setoutput(pin) __asm__ volatile ("dirh %0" : : "r" (pin) : )
#define sethi(pin)     __asm__ volatile ("outh %0" : : "r" (pin) : )
#define setlo(pin)     __asm__ volatile ("outl %0" : : "r" (pin) : )
/* LLVM doesn't support getct instruction yet, so use:
       fd63ec1a  getct pa
       fc67ec10  wrlong pa, 0x10
   to write the current counter to memory, from whence we can grab it
*/

#define storecnt()      __asm__ volatile (".long 0xfd63ec1a ;  .long 0xfc67ec10" : : : )
static unsigned long hackct()
{
    storecnt();
    return *(unsigned long *)0x10;
}

#define getcnt(x)       (x = hackct())

/* LLVM has no libraries yet, really... */
// int light_printf(const char *fmt, ...);
// #define printf light_printf

#else

#include <stdint.h>
#include <propeller2.h>
#include <stdio.h>

#define setoutput(x)
#define sethi(x)    _pinh(x)
#define setlo(x)    _pinl(x)
#define getcnt(x)   (x = _cnt())
#endif

/**************************************************************************************
 * spiWriteReg
 *
 * Writes to an 8-bit register with the SPI port
 * Adapted from "4184_spi.c
 **************************************************************************************/

void spiWriteData(const unsigned char regData)
{

  int SPICount;                               // Counter used to clock out the data

  unsigned char SPIData;                                // Define a data structure for the SPI data.

  setoutput(SPI_MOSI);
  sethi(SPI_CS);                                           // Make sure we start with /CS high
  setlo(SPI_CK);                                           // and CK low

  SPIData = regData;                                    // Preload the data to be sent with Address
  setlo(SPI_CS);                                           // Set /CS low to start the SPI cycle 25nS

  for (SPICount = 0; SPICount < 8; SPICount++)          // Prepare to clock out the Address byte
  {
    if (SPIData & 0x80)                                 // Check for a 1
        sethi(SPI_MOSI);                                     // and set the MOSI line appropriately
    else
        setlo(SPI_MOSI);
    sethi(SPI_CK);                                    // Toggle the clock line
    setlo(SPI_CK);
    SPIData <<= 1;                                      // Rotate to get the next bit

  }                                                     // and loop back to send the next bit
  sethi(SPI_CS);
  setlo(SPI_MOSI);
}

unsigned char msg[512];

int main()
{
    int i;
    unsigned long startcycle, endcycle;

#ifdef __p2llvm__
    //extern int uart_init(int, int, int);
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(63, 62, 230400);
#endif
    printf("SPI simulation on pins %d to %d\n", BASEPIN, BASEPIN+3);

    setoutput(SPI_CS);
    setoutput(SPI_CK);

    getcnt(startcycle);
    for (i = 0; i < 512; i++) {
        spiWriteData(msg[i]);
    }
    getcnt(endcycle);

    printf("%lu cycles elapsed\n", endcycle - startcycle);
    for(;;) {
    }
    return 0;
}
