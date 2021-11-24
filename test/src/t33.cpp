#include <stdio.h>
#include <stdint.h>

int main() {
    printf("$\n"); // start of test character
    
    // uint64_t a = 0x800000000;
    // uint64_t b = 0x2;

    // uint64_t c = a+b;

    // printf("%x, %x\n", c >> 32, c & 0xffffffff);

    printf("%llx\n", 0x100000000);

    printf("~\n"); // end of test character

    while(1);
}