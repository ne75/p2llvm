#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

__attribute__ ((noreturn)) void __fcacher() {
    asm volatile (
        // compute slot address
        "and pa, #7\n"          // 180: get lowest 3 bits as the address into the cache map
        "shl pa, #1\n"          // 181: double
        "mov $0x1b0, #0x1c0\n"  // 182: 
        "add $0x1b0, pa\n"      // 183: add 1c0. this register now holds the value of the register that holds the address of the 
        "alts $0x1b0, #0\n"     // 184
        "mov $0x1b0, #0\n"      // 185


                                // 181:


                                // -- variables --
                                // 1b0: cache_addr_loc/func addr

                                // 1c0: start of cache table
                                // ...
                                // 1cf: last address available, 1d0 is r0. don't touch r0 or higher
        "reta\n"
    );
}

#define INIT_CACHE asm("setq #2\n rdlong $0x180, %0\n" ::"r"((int)__fcacher))

__attribute__ ((noinline, cogcache)) int cached_func() {
    volatile int x = 123;
	return x;
}

int main(void) {
    INIT_CACHE;
    // printf("$\n");
	
	// printf("%d\n", cached_func());

    // printf("~\n"); // end of test character

    volatile int x = cached_func();

    printf("%d\n", x);

    while (1) {
        waitx(1000);
    }
}