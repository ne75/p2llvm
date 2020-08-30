// due to current limitations of the ASM parser, need to manually calculate jump offsets.
// eventually need label support so that jumps can be done much more easily

#include "propeller2.h"

__attribute__ ((section (".stack"))) unsigned int __stack;

__attribute__ ((section ("cog"))) void __unreachable();
__attribute__ ((section ("cog"))) void __start();
__attribute__ ((section ("cog"))) int __sdiv(int a, int b);
__attribute__ ((section ("cog"))) int __srem(int a, int b);

extern int main();
extern void _cstd_init();

void __entry() {
    // basic entry code to jump to our resuable startup code. we do this by restarting cog 0, copying in
    // the code in the cog section (our reusable startup code). The linker will place _start() at address 0x100

    // this function will get overwritten later by hub params (clkfreq, clkmode, etc)
    asm("coginit #0, #0x100");
}

// I eventually want to figure out how to do labels in the assembly parser so that I don't need to pre-compute jump offsets
// can also probably re-write this to always run the not-cog 0 version of the startup and do the cog 0 startup stuff in
// _entry()

void __start() {
    // TODO: figure out how to rewrite this without needing inline asm.
    asm("cogid $r0\n"           // get the current cog ID
        "tjz $r0, #5\n"         // if cog 0, jump to the special cog0 startup code.

        "mov $r0, $ptra\n"      //  if not cog 0, save ptra (value at ptra should be pointer to start of stack)
        "rdlong $r1, $r0\n"     //  read out first stack value
        "add $r0, #4\n"
        "rdlong $r0, $r0\n"     //  read out second stack value
        "jmp $r1\n");           //  jump to the cog function

    // initialize the stack
    PTRA = (unsigned int)&__stack;

    // initialize the C std library, if linked
    _cstd_init();

    // run the main function
    main();

    // loop forever
    __unreachable();
}

// a give-up function if we have some fatal error.
void __unreachable(int status) {
    while(1);
}

int __sdiv(int a, int b) {
    // write this partially as a normal C function for now, optimize with assembly later
    // full assembly optimization will require adding conditional statement parsing.
    // the alternative until we get there is to pre-compile asm functions with fast spin
    // and link the binary. Otherwise, we are using up cycles pushing and popping registers
    // to and from the stack
    int result_neg = (a ^ b) >> 31;

    // faster than using abs() function
    asm("abs %0, %1" : "=r"(a) : "r"(a));
    asm("abs %0, %1" : "=r"(b) : "r"(b));

    int res = (unsigned int)a/(unsigned int)b;

    if (result_neg) return -res;

    return res;
}

int __srem(int a, int b) {
    // https://llvm.org/docs/LangRef.html#srem-instruction
    // per LLVM instruction set, srem return should have the same sign as the first operand, a
    int result_neg = a >> 31;

    // faster than using abs() function
    asm("abs %0, %1" : "=r"(a) : "r"(a));
    asm("abs %0, %1" : "=r"(b) : "r"(b));

    int res = (unsigned int)a%(unsigned int)b;

    if (result_neg) return -res;

    return res;
}