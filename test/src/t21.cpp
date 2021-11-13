#include <stdio.h>

void recurse(int i) {
    if (i == 0) return;

    printf("%d\n", i);

    recurse(i-1);
}

int main() {
    printf("$\n"); // start of test character

    recurse(10);

    printf("~\n"); // end of test character
    return 0;
}