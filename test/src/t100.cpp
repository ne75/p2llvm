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

inline void _dirh(char pin) {
    asm volatile  ("dirh %0\n"::"r"(pin));
}

inline void _dirl(char pin) {
    asm volatile  ("dirl %0\n"::"r"(pin));
}

inline void _pinh(char pin) {
    asm volatile  ("dirh %0\n"::"r"(pin));
}

inline void _pinl(char pin) {
    asm volatile  ("dirl %0\n"::"r"(pin));
}

inline void _pinnot(char pin) {
    asm volatile  ("outnot %0\n"::"r"(pin));
}

inline int _pinr(char pin) {
    int state;

    asm volatile ("testp %1 wc\n"
        "wrc %0\n"
        :"+r"(state)
        :"r"(pin));
    return state;
}

/**
 * @brief test the logic state
 * @param pin pin to test
 */
inline int _testp(char pin) {
    int rslt;

    asm volatile ("testp %1 wc\n"
        "wrc %0\n"
        :"=r"(rslt)
        :"r"(pin));
    return rslt;
}

void _waitus(unsigned s) {
   asm("waitx %0" :: "r"(s));
}

int get_p_test(int d, int c, int s) {
   _dirl(d); // float answer
   _waitus(s);
   _pinh(c);
   _waitus(s);
   d = _pinr(d); //ACK=low
   _pinl(c);
   _waitus(s);
   return d;
}

int main() {
   _clkset(_SETFREQ, _CLOCKFREQ);
   _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);

   get_p_test(1, 2, 100);
   
   while(1) {
      waitcnt(CLKFREQ + CNT);
   }
}