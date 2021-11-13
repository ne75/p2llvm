#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    for (int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    int i = 0;
    while (i < 5) {
        printf("%d\n", i++);
    }

    i = 0;
    do {
        printf("%d\n", i++);
    } while (i < 5);

    printf("~\n"); // end of test character
    return 0;
}