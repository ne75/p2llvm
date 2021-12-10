#include <stdio.h>
#include <stdint.h>
#include <debug.h>

volatile double a = 2.0;
volatile double b = 5.0;
volatile double d = -1.0;
int main() {
    // brk(DEBUG_CODE_INIT);
    printf("$\n"); // start of test character

    if (a < 3.0)
        printf("a < 3.0\n");

    if (a > 3.0)
        printf("a > 3.0\n");

    if (b < 3.0)
        printf("b < 3.0\n");

    if (b > 3.0)
        printf("b > 3.0\n");

    if (b < 0.0) 
        printf("b < 0.0\n");

    // limit to 9 bit for printing since more than that will have division error in the optimized vs non-optimized case 
    // the optimized case converts division to multiplication which is more accurate
    double c = a + b;
    printf("%.9f\n", c);

    c = a*b;
    printf("%.9f\n", c);

    c = a/4.0;
    printf("%.9f\n", c);

    c *= d;
    printf("%.9f\n", c);

    printf("~\n"); // end of test character

    busywait();
}