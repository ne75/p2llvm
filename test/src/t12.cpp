#include <stdio.h>

void a() {
    printf("a()\n");
}

int main() {
    printf("$\n"); // start of test character

    a();

    printf("~\n"); // end of test character
    return 0;
}