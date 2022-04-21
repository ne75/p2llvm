#include <propeller.h>
#include <stdio.h>

unsigned char b1[13], b2[13];

int main() {
    printf("$\n");
    const char *s = "Hello world!";

    memset(b2, 1, 13);
    
    for (int i = 0; i < 13; i++)
        printf("%d ", b2[i]);

    printf("\n");

    memcpy(b1, b2, 13);

    for (int i = 0; i < 13; i++)
        printf("%d ", b1[i]);

    printf("\n");

    memmove(b2, s, 13);
    printf("%s\n", b2);

    printf("~\n");

    while (1) {
        waitx(500);
    }
}
