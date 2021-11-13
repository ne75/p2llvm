#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    DIRA = 10;
    printf("%d\n", DIRA); // expect 10
    
    DIRA = 0;
    OUTA = 0;
    dirh(3);
    printf("%d\n", DIRA); // expect 8

    outh(3);
    printf("%d\n", OUTA); // expect 8

    printf("%d\n", INA & 0x8); // expect 8

    printf("~\n"); // end of test character
    return 0;
}