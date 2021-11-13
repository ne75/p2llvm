#include <stdio.h>

void a(int a, int b) {
    printf("a(%d, %d)\n", a, b);
}

int main() {
    printf("$\n"); // start of test character

    a(1, 2);

    printf("~\n"); // end of test character
    return 0;
}