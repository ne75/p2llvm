#include <propeller.h>

struct led_mb_t {
    char pin;
    int delay;
};

unsigned int stack1[20];
unsigned int stack2[20];
unsigned int stack3[20];

void blink(void *par) {
    led_mb_t *led = (led_mb_t*)par;

    dirh(led->pin);

    while(1) {
        outnot(led->pin);
        waitx(led->delay);
    }
}

int main() {
    led_mb_t led1 = {56, CLKFREQ};
    led_mb_t led2 = {57, CLKFREQ/2};
    led_mb_t led3 = {58, CLKFREQ/4};

    cogstart(blink, (int)&led1, (int*)stack1, sizeof(stack1));
    cogstart(blink, (int)&led2, (int*)stack2, sizeof(stack2));
    cogstart(blink, (int)&led3, (int*)stack3, sizeof(stack3));

    while(1) {
        waitx(CLKFREQ);
    }
}