#include <stdio.h>

int a() {
    printf("a()\n");
    return 1;
}

int main() {
    printf("$\n"); // start of test character

    int b = a();
    printf("%d\n", b);

    printf("~\n"); // end of test character
    return 0;
}