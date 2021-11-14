#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    volatile int a = 2;
    volatile int b = 3;

    int c = a + b;
    printf("%d ", c); // 1. expect 5

    c = a - b;
    printf("%d ", c); // 2. expect -1

    c = -a;
    printf("%d ", c); // 3. expect -2

    c = a * b;
    printf("%d ", c); // 4. expect 6

    c = -a * b;
    printf("%d ", c); // 5. expect -6

    c = b / a;
    printf("%d ", c); // 6. expect 1

    c = -b / a;
    printf("%d ", c); // 7. expect -1

    c = a * b;
    printf("%d ", c); // 8. expect 6

    c = ++a;
    printf("%d ", c); // 9. expect 3

    c = --a;
    printf("%d ", c); // 10. expect 2

    c = ~a;
    printf("%u ", c); // 11. expect 4294967293
    c = 2;

    c = a & b;
    printf("%d ", c); // 12. expect 2

    c = a | b;
    printf("%d ", c); // 13. expect 3
    
    c = a ^ b;
    printf("%d ", c); // 14. expect 1

    c = a << b;
    printf("%d ", c); // 15. expect 16

    c = c >> b;
    printf("%d ", c); // 16. expect 2

    printf("\n~\n"); // end of test character
    return 0;
}