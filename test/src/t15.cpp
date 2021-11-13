#include <stdio.h>

void a(int a, int b, int c, int d, int e) {
    printf("a(%d, %d, %d, %d, %d)\n", a, b, c, d, e);
}

int main() {
    printf("$\n"); // start of test character

    a(1, 2, 3, 4, 5);

    printf("~\n"); // end of test character
    return 0;
}