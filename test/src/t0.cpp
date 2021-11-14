#include <propeller.h>

int main() {
    while(1) {
        drvnot(56);
        waitx(CLKFREQ);
    }
    return 0;
}