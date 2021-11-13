#include <stdio.h>

struct s_t {
    int a, b;
};

void a(int a, int b, int c, int *d, s_t &e) {
    printf("a(%d, %d, %d, %d, {%d, %d})\n", a, b, c, *d, e.a, e.b);
}

int b, c, d, e;

int main() {
    printf("$\n"); // start of test character

    s_t f;
    b = 1;
    c = 2;
    d = 3;
    e = 4;
    f.a = 5;
    f.b = 6;

    a(b, c, d, &e, f);

    printf("~\n"); // end of test character
    return 0;
}