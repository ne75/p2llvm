/*
  This tests variadic functions. One of the last things needed before we can begin implementing a much more complex library
  Also, this expands to start introducing an implementation of the C standard library

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"

#define RX_PIN 63
#define TX_PIN 62

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v,l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_copy(d,s)   __builtin_va_copy(d, s)

volatile unsigned int blink1_stack[32];
volatile unsigned int blink2_stack[32];
volatile unsigned int blink3_stack[32];
volatile unsigned int blink4_stack[32];

volatile int uart_clock_per_bits;

void uart_str(const char *str) __attribute__((noinline));
void uart_int(int n, unsigned int base) __attribute__((noinline));

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

void uart_int(int n, unsigned int base) {
    char tmp[12]; // we'll never have more than 10 digits for a 32 bit number in base 10 (including negative), plus 1 for 0 termination
    int i = 11;
    bool is_neg = false;
    if (n < 0) {
        n = -n;
        is_neg = true;
    }

    const char *chars = "0123456789abcdef";

    tmp[i--] = 0; // 0 terminate the string
    do {
        tmp[i--] = chars[(n%base)&0xf]; // mask the bits to make sure we don't index something outside of the range of chars
        n = n/base;
    } while (n > 0);

    if (is_neg) tmp[i--] = '-';
    uart_str(&tmp[i+1]);
}

struct led_mb_t {
    char pin;
    int delay;
    unsigned int *stack;
};

void blink(void *par) {
    led_mb_t *led = (led_mb_t*)par;

    dirh(led->pin);

    while(1) {
        outl(led->pin);
        waitx(led->delay);
        outh(led->pin);
        waitx(led->delay);
    }
}

void start_blinks(led_mb_t *led, ...) {
    va_list args;
    va_start(args, led);

    led_mb_t *l = led;
    while(l != 0) {
        cognew(blink, (int)l, (unsigned int*)(l->stack));
        l = va_arg(args, led_mb_t*);
    }
}

// sum n numbers
int sum(int n, ...) {
    va_list args;
    va_start(args, n);

    int v = 0;

    for (int i = 0; i < n; i++) {
        v += va_arg(args, int);
    }

    return v;
}

int main() {
    clkset(_SETFREQ, _CLOCKFREQ);
    uart_clock_per_bits = uart_init(RX_PIN, TX_PIN, 230400);

    // uart_str("Variadic function test\n");

    // // led_mb_t led1 = {56, 20000000, (unsigned int*)blink1_stack};
    // // led_mb_t led2 = {57, 20000000/2, (unsigned int*)blink2_stack};
    // // led_mb_t led3 = {58, 20000000/3, (unsigned int*)blink3_stack};
    // // led_mb_t led4 = {59, 20000000/4, (unsigned int*)blink4_stack};

    // See wiki for why I'm doing things this way
    led_mb_t led1 = {56, _CLOCKFREQ, 0};
    led_mb_t led2 = {57, _CLOCKFREQ/2, 0};
    led_mb_t led3 = {58, _CLOCKFREQ/3, 0};
    led_mb_t led4 = {59, _CLOCKFREQ/4, 0};

    led1.stack = (unsigned int*)blink1_stack;
    led2.stack = (unsigned int*)blink2_stack;
    led3.stack = (unsigned int*)blink3_stack;
    led4.stack = (unsigned int*)blink4_stack;

    start_blinks(&led1, &led2, &led3, &led4, 0);

    int x = sum(5, 10, 20, -1, 34, -24);
    uart_int(x, 10);
    uart_str("\r\n");

    uart_int(125, 10);

    while(1);
    return 0;
}