#include <stdio.h>

void a(bool one=false) {
    printf("1\n");

    if (one) return;

    printf("2\n");
}

int main() {
    printf("$\n"); // start of test character

    a();
    a(true);

    printf("~\n"); // end of test character
    return 0;
}