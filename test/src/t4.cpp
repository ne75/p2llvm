#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    int a[3];

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    printf("%d %d %d", a[0], a[1], a[2]);

    printf("\n~\n"); // end of test character
    return 0;
}