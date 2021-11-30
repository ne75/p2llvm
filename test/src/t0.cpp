#include <propeller.h>

int main() {
    brk(0);
    volatile int x = 0;
    x = 10000;
    return 0;
}