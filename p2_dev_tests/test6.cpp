/*
  This tests the assembly parser and instruction encoder to generate usable code. Also, this step introduced the clang
  integration so that code can be directly output as an encoded object. The linker is also extended to support P2, and a
  simple linker script is used to create an executable elf where fixups are applied

  compilation:
    ../build/bin/clang -ffunction-sections --target=p2 -integrated-as -c test6.cpp -o test6.o
    ../build/bin/ld.lld -Tp2.ld -o test6 test6.o
*/

#define waitx(t) asm("waitx %0" : : "r"(t))
#define dirh(pin) asm("dirh %0" : : "r"(pin))
#define dirl(pin) asm("dirl %0" : : "r"(pin))
#define outh(pin) asm("outh %0" : : "r"(pin))
#define outl(pin) asm("outl %0" : : "r"(pin))

// void blink() {
//     outh(56);
//     waitx(20000000);
//     outl(56);
//     waitx(20000000);
// }

int main() {

    dirh(56);
    outl(56);

    while(1) {
        //waitx(20000000);
        //blink();
    }

    return 0;
}