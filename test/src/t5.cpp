#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    struct s_t {
        int a;
        short b;
        char c;
    };

    s_t s = {1, 2, 3};
    printf("%d %d %d\n", s.a, s.b, s.c);

    s_t t = s;
    printf("%d %d %d\n", t.a, t.b, t.c);

    printf("~\n"); // end of test character
    return 0;
}