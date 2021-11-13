#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    struct s_t {
        int a;
        int b;
    };

    s_t s = {2, 3};
    printf("%d %d\n", s.a, s.b);

    s_t t = s;
    printf("%d %d\n", t.a, t.b);

    printf("~\n"); // end of test character
    return 0;
}