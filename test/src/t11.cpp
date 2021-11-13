#include <stdio.h>

class A {
public:
    int a;
    int b[2];
    static int c;

    A(int _a, int _b, int _c) {
        a = _a;
        b[0] = _b;
        b[1] = _c;
    };
    
    ~A() {};
};

int A::c = 3;

A s(1, 2, 4);

int main() {
    printf("$\n"); // start of test character

    printf("%d %d %d %d\n", s.a, s.b[0], s.b[1], s.c);

    printf("~\n"); // end of test character
    return 0;
}