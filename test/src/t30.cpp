#include <stdio.h>

struct a {
    int x[1000];
    int y;
};

a t;

int main() {
    printf("$\n"); // start of test character

    t.x[0] = 1;
    t.y = 2;

    printf("%d\n", t.x[0]);
    printf("%d\n", t.y);

    printf("~\n"); // end of test character
    return 0;
}