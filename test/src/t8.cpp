#include <stdio.h>

int main() {
    printf("$\n"); // start of test character

    int a = 0;
    int b = 1;

    if (a) {
        printf("do not print\n");
    } else {
        printf("not a = 0\n");
    }

    if (b) {
        printf("b = 1\n");
    } else {
        printf("do not print\n");
    }

    if (a == b) {
        printf("do not print\n");
    } else {
        printf("not a == b\n");
    }

    if (a != b) {
        printf("a != b\n");
    } else {
        printf("do not print\n");
    }

    if (a > b) {
        printf("do not print\n");
    } else {
        printf("not a > b\n");
    }

    if (a < b) {
        printf("a < b\n");
    } else {
        printf("do not print\n");
    }

    if (a >= b) {
        printf("do not print\n");
    } else {
        printf("not a >= b\n");
    }

    if (a <= b) {
        printf("a <= b\n");
    } else {
        printf("do not print\n");
    }

    printf("~\n"); // end of test character
    return 0;
}