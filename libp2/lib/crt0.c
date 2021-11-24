/**
 * All of the core code necessary to run our compiled binaries
 */

#include "propeller2.h"
#include "debug.h"

typedef void (*func_ptr)(void);

extern unsigned int __stack;

__attribute__ ((section ("cog"), cogmain)) void __start();

// the relocation for these calls isn't encoded properly since the linker thinks the calls are global calls, not external "libcalls"
void _init();
void _fini();

// These are our built-in functions that LLVM can use. Cogtext attribute does nothing, but including it for future use
__attribute__ ((section ("lut"), cogtext)) int __divsi3(int a, int b);
__attribute__ ((section ("lut"), cogtext)) int __modsi3(int a, int b);
__attribute__ ((section ("lut"), cogtext)) long long __udivdi3(long long a, long long b);
__attribute__ ((section ("lut"), cogtext)) long long __umoddi3(unsigned long long a, unsigned long long b);
__attribute__ ((section ("lut"), cogtext)) long long __divdi3(unsigned long long a, unsigned long long b);
__attribute__ ((section ("lut"), cogtext)) long long __moddi3(unsigned long long a, unsigned long long b);
__attribute__ ((section ("lut"), cogtext)) long long __muldi3(long long a, long long b);
__attribute__ ((section ("lut"), cogtext)) long long __lshrdi3(long long a, int n);
__attribute__ ((section ("lut"), cogtext)) void *__memcpy(void *dst, const void *src, unsigned n);
__attribute__ ((section ("lut"), cogtext)) void *__memset(void *str, int c, unsigned n);

extern int main();
extern void _cstd_init();
extern func_ptr _init_array_start[];
extern func_ptr _init_array_end[];
extern func_ptr _fini_array_start[];
extern func_ptr _fini_array_end[];
extern unsigned _libcall_start[];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
__attribute__ ((cogmain, noreturn)) void __entry() {
    // basic entry code to jump to our resuable startup code. we do this by restarting cog 0, copying in
    // the code in the cog section (our reusable startup code). The linker will place _start() at address 0x100

    // this function might get overwritten later by hub params (clkfreq, clkmode, etc), so DO NOT try to restart the code with coginit #0, #0

    // before we start the routine, enable debugging for cog 0. any other cogs that want to be debugged should be enabled by the application
    hubset(DEBUG_INT_EN | DEBUG_COG0);
    asm("coginit #0, %0" : : "r"(__start));
}
#pragma clang diagnostic pop

void __start() {
    // TODO: figure out how to rewrite this without needing inline asm.
    INIT_RTLIB;

    asm("cogid $r0\n"           // get the current cog ID
        "tjz $r0, #.Linit\n"    // if cog 0, jump to the special cog0 startup code.

        "mov $r0, $ptra\n"      //  if not cog 0, save ptra (value at ptra should be pointer to start of stack)
        "rdlong $r1, $r0\n"     //  read out first stack value
        "add $r0, #4\n"
        "rdlong $r0, $r0\n"     //  read out second stack value
        "jmp $r1\n"             //  jump to the cog function
        ".Linit:"
    );

    // initialize the stack
    PTRA = (unsigned int)&__stack;

    // init the debug lock
    _dbg_lock = _locknew();

    // default clock frequency. Assumes the loader does not try to set a different one.
    // if we want the loader to be able to configure it, we'll need to pull this number from some shared memory location
    _clkfreq = 24000000;

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
    for (func_ptr* func = _init_array_start; func != _init_array_end; func++) {
        (*func)();
    }
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

/**
 * return quotient of signed of a/b
 */ 
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
int __divsi3(int a, int b) {
    // r0 = a, r1 = b
    asm(
               "mov $r2, $r0\n"
               "xor $r2, $r1\n"
               "shr $r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
               "abs $r0, $r0\n"
               "abs $r1, $r1\n"
               "abs $r0, $r0\n"
               "qdiv $r0, $r1\n"
               "getqx $r31\n"
        "if_nz  neg $r31, $r31\n"
    );   
}
#pragma clang diagnostic pop

/**
 * return remainder of signed a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
int __modsi3(int a, int b) {
    // https://llvm.org/docs/LangRef.html#srem-instruction
    // per LLVM instruction set, srem return should have the same sign as the first operand, a
    
    // r0 = a, r1 = b
    asm(
               "mov $r2, $r0\n"
               "shr $r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
               "abs $r0, $r0\n"
               "abs $r1, $r1\n"
               "abs $r0, $r0\n"
               "qdiv $r0, $r1\n"
               "getqx $r31\n"
        "if_nz  neg $r31, $r31\n"
    );  
}
#pragma clang diagnostic pop

/**
 * return quotient of unsigned a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __udivdi3(long long a, long long b) {
    asm(
            ""
    );
}
#pragma clang diagnostic pop

/**
 * return remainder of unsigned a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __umoddi3(unsigned long long a, unsigned long long b) {
    asm(
        ""
    );
}
#pragma clang diagnostic pop


/**
 * return quotient of signed a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __divdi3(unsigned long long a, unsigned long long b) {
    asm(
            ""
    );
}
#pragma clang diagnostic pop

/**
 * return remainder of signed a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __moddi3(unsigned long long a, unsigned long long b) {

}
#pragma clang diagnostic pop

/**
 * return product of signed a*b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __muldi3(long long a, long long b) {
    // let a = a0 + a1 << 32, same for b
    // a*b = a0*b0 + (a1*b0 + a0*b1) << 32 (the a1*b1 term gets shifted by 64 and is therefore always 0)
    // multiply the 3 parts, then get qx/qy for the first one, then get qx into the upper part of the result
    // to shift by 32 right away

    asm(
        // a0*b0
        "qmul $r0, $r2\n"
        "getqx $r30\n"
        "getqy $r31\n"

        // a1*b0
        "qmul $r1, $r2\n"
        "getqx $r3\n" // r3 = upper 32 of a1*b0 << 32
        
        // a0*b1
        "qmul $r0, $r3\n"
        "getqx $r4\n" // r4 = upper 32 of a0*b1 << 32

        // add up
        "add $r31, $r3\n"
        "add $r31, $r4\n"
    );
}
#pragma clang diagnostic pop


/**
 * return logical shift right of a by n
 * 
 * a is stored in r0 and r1. n is in r2
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
long long __lshrdi3(long long a, int n) {
    asm(
        "cmp $r2, #32\n"
        "if_nc jmp #.Lge32\n"
        // if n < 32
        "mov $r3, #32\n"
        "sub $r3, $r2\n"
        
        // hi = hi_in >> n
        // lo = lo_in >> n | (hi_in << (32-n))

        "mov $r31, $r1\n"
        "shr $r31, $r2\n"

        "mov $r30, $r0\n"
        "shr $r30, $r2\n"
        "shl $r1, $r3\n"
        "or $r30, $r1\n"

        "reta\n"

        ".Lge32:\n"
        // n >= 32
        "mov $r3, $r2\n"
        "sub $r3, #32\n"

        // hi = 0
        // lo = hi_in >> (n-32)
        "mov $r31, #0\n"
        "mov $r30, $r1\n"
        "shr $r30, $r3\n"
    );
}
#pragma clang diagnostic pop

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