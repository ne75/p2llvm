#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    for (int i = 0; i < 6; i++) {
        switch (i) {
            case 0:
                printf("case 0\n");
                break;
            case 1:
                printf("case 1\n");
                break;
            case 2:
                printf("case 2 with fallthrough to ");
            case 3:
                printf("case 3\n");
                break;
            case 4:
                printf("case 4\n");
                break;
            default:
                printf("unknown case\n");
        }
    }

    printf("~\n"); // end of test character
    return 0;
}