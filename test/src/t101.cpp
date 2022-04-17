#include <propeller.h>
#include <stdio.h>
#include <stdint.h>

void SendSD(int drive, const unsigned char* buff, unsigned int bc);

int main(int argc, char** argv) {
    unsigned char *b1, *b2;

    SendSD(0, b1, 1);

    while (1) {
        waitx(500);
    }
}

void SendSD(int drive, const unsigned char* buff, unsigned int bc) __attribute__ ((noinline)) {
    int mosi = 10; //(Pins[drive] >> 16) & 0xff;
    int clk = 11;  //(Pins[drive] >> 8) & 0xff;
    int i,j,x;

    asm volatile ("mov %[i], %[bc]\n"
             ".Li: mov %[j], #8\n"
                  "rdbyte %[x], %[b]\n"
                  "shl %[x], #24\n"
             ".Lj: shl %[x], #1 wc\n"
             "if_c drvh %[mosi]\n"
            "if_nc drvl %[mosi]\n"
                  "drvh %[clk]\n"
                  "waitx #2\n"
                  "drvl %[clk]\n"
                  "waitx #2\n"
                  "djnz %[j], #.Lj\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li\n"
                  :[b]"+&r"(buff), [i]"+&r"(i), [j]"+&r"(j), [x]"+&r"(x)
                  :[mosi]"r"(mosi), [clk]"r"(clk), [bc]"r"(bc));

}