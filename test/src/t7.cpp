#include <stdio.h>

class A {
public:
    A() {};
    ~A() {};
    virtual void a() = 0;
};

class B : public A {
public:
    B() {};
    ~B() {};

    void a() override {
        printf("virtual a()\n");
    }
};

int main() {
    printf("$\n"); // start of test character

    B b;

    b.a();

    printf("~\n"); // end of test character
    return 0;
}