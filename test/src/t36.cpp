#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("$\n");

	printf("%s \n%c \n%d \n%lld \n%u \n%llu \n%f \n%.9f \n%x\n",
		"hello world!",
		'c',
		-1234,
		-1234123412341234,
		1234,
		1234123412341234,
		1.234f,
		-1.234567890,
		0xdeadbeef
		);

    printf("~\n"); // end of test character

    while (1) {
        waitx(1000);
    }
    
}