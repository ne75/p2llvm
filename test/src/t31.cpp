#include <stdio.h>

struct s_t {
    char a;
    int b;
};

s_t a(int a, int b, int c, int d, int e) {
    s_t r = {1, 2};
    return r;
}

int main() {
    printf("$\n"); // start of test character

    s_t r = a(1, 2, 3, 4, 5);
    printf("%d %d\n", r.a, r.b);

    printf("~\n"); // end of test character
    return 0;
}