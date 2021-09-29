/**
 * All of the core code necessary to run our compiled binaries
 */

#include "propeller2.h"

typedef void (*func_ptr)(void);

__attribute__ ((section (".stack"))) unsigned int __stack;

__attribute__ ((section ("cog"), cogmain)) void __start();
__attribute__ ((section ("cog"))) void _init();
__attribute__ ((section ("cog"))) void _fini();

// These are our built-in functions that LLVM can use
__attribute__ ((section ("cog"))) int __sdiv(int a, int b);
__attribute__ ((section ("cog"))) int __srem(int a, int b);
__attribute__ ((section ("cog"))) void *__memcpy(void *dst, const void *src, unsigned n);
__attribute__ ((section ("cog"))) void *__memset(void *str, int c, unsigned n);

extern int main();
extern void _cstd_init();
extern func_ptr _init_array_start[];
extern func_ptr _init_array_end[];
extern func_ptr _fini_array_start[];
extern func_ptr _fini_array_end[];

__attribute__ ((cogmain)) void __entry() {
    // basic entry code to jump to our resuable startup code. we do this by restarting cog 0, copying in
    // the code in the cog section (our reusable startup code). The linker will place _start() at address 0x100

    // this function will get overwritten later by hub params (clkfreq, clkmode, etc)
    asm("coginit #0, #0x100");
}

void __start() {
    // TODO: figure out how to rewrite this without needing inline asm.

    asm("cogid $r0\n"           // get the current cog ID
        "tjz $r0, #.Linit\n"         // if cog 0, jump to the special cog0 startup code.

        "mov $r0, $ptra\n"      //  if not cog 0, save ptra (value at ptra should be pointer to start of stack)
        "rdlong $r1, $r0\n"     //  read out first stack value
        "add $r0, #4\n"
        "rdlong $r0, $r0\n"     //  read out second stack value
        "jmp $r1\n"             //  jump to the cog function
        ".Linit:"
    );

    // initialize the stack
    PTRA = (unsigned int)&__stack;

    // setup c standard library
    _cstd_init();

    // initialize all our constructors
    _init();

    // run the main function
    main();

    // in theory, we don't need this, but leave it for completeness
    _fini();

    // loop forever
    __unreachable();
}

// got this implementation from https://wiki.osdev.org/Calling_Global_Constructors
void _init(void) {
    for (func_ptr* func = _init_array_start; func != _init_array_end; func++)
        (*func)();
}

void _fini(void) {
    for (func_ptr* func = _fini_array_start; func != _fini_array_end; func++)
        (*func)();
}

// a give-up function if we have some fatal error.
void __unreachable() {
    while(1);
}

void __cxa_pure_virtual() {
    __unreachable();
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

__attribute__ ((no_builtin("memcpy"))) void *__memcpy(void *dst, const void *src, unsigned n) {
    char *d = (char *)dst;
    const char *s = (const char*)src;

    // might be a smarter way to do this using the FIFO and such
    if ((d != 0) && (s != 0)) {
        while(n) {
            //Copy byte by byte
            *(d++)= *(s++);
            --n;
        }
    }

    return dst;
}

__attribute__ ((no_builtin("memset"))) void *__memset(void *dst, int c, unsigned n) {
    // this can certainly be rewritten using the FIFO with wfbyte
    char *d = (char *)dst;
    if (d != 0) {
        for (int i = 0; i < n; i++) {
            d[i] = c;
        }
    }

    return dst;
}