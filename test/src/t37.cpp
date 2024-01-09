#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float fs[] = {
	1.0f,
	2.0f,
	25.0,
	4294967295.0,
	0.0f,
	-0.0f,
	INFINITY,
	-INFINITY,
	0.0f/0.0f,
	-1.0
};

int main(void) {
    printf("$\n");
	
	for (int i = 0; i < sizeof(fs)/sizeof(float); i++) {
		printf("%f\n", sqrtf(fs[i]));
	}

    printf("~\n"); // end of test character

    while (1) {
        waitx(1000);
    }
    
}