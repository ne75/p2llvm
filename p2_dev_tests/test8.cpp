/*
  This tests UART printing

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"

#define RX_PIN 63
#define TX_PIN 62

unsigned int blink1_stack[32];
unsigned int blink2_stack[32];

struct led_mb_t {
    char pin;
    int delay;
};

volatile int uart_clock_per_bits;

void uart_str(const char *str) __attribute__((noinline));
void uart_dec(int n) __attribute__((noinline));

void blink(void *par) {
    led_mb_t *led = (led_mb_t*)par;

    dirh(led->pin);

    while(1) {
        outh(led->pin);
        waitx(led->delay);
        outl(led->pin);
        waitx(led->delay);
    }
}

/*
 * print a 0 terminated string
 */
void uart_str(const char *str) {
    while (*str) {
        uart_putc(*str);
        str++;
        waitx(uart_clock_per_bits*10); // wait for the bits to send
    }
}

void uart_dec(int n) {
    char tmp[11]; // we'll never have more than 10 digits for a 32 bit number in base 10 (including negative), plus 1 for 0 termination
    int i = 10;
    bool is_neg = false;
    if (n < 0) {
        n = -n;
        is_neg = true;
    }

    tmp[i--] = 0; // 0 terminate the string
    do {
        tmp[i--] = '0' + (n % 10);
        n = n/10;
    } while (n != 0);

    if (is_neg) tmp[i--] = '-';
    uart_str(&tmp[i+1]);
}

//led_mb_t led1;

int main() {

    clkset(_SETFREQ, _CLOCKFREQ);

    //waitx(_CLOCKFREQ/10);

    uart_clock_per_bits = uart_init(RX_PIN, TX_PIN, 230400);

    uart_str("Hello World!\n");
    led_mb_t led1 = {56, _CLOCKFREQ};
    led_mb_t led2 = {58, _CLOCKFREQ/2};

    cognew(blink, (int)&led1, blink1_stack);
    cognew(blink, (int)&led2, blink2_stack);

    uart_dec(_clkfreq);

    while(1);

    return 0;
}