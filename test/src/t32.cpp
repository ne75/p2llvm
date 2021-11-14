#include <stdio.h>

int c;

int main() {
    printf("$\n"); // start of test character

    volatile int a = 1000;
    volatile int b = 2000;

    printf("%d\n", a+b);

    c = 4000;
    printf("%d\n", c);

    printf("~\n"); // end of test character
    return 0;
}