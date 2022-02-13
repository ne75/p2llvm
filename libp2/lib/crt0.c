/**
 * All of the core code necessary to run our compiled binaries
 */

#include "propeller2.h"
#include "debug.h"

typedef void (*func_ptr)(void);

extern unsigned int __stack;
int argc = 1;
static char *argv[2];
static char program[] = "P2LLVM";
static char _dummy[] = "dummy";

__attribute__ ((section ("cog"), cogmain)) void __start();

// the relocation for these calls isn't encoded properly since the linker thinks the calls are global calls, not external "libcalls"
void _init();
void _fini();

extern int main();
extern void _cstd_init();
extern func_ptr _init_array_start[];
extern func_ptr _init_array_end[];
extern func_ptr _fini_array_start[];
extern func_ptr _fini_array_end[];
extern unsigned _libcall_start[];
extern int __enable_p2db;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
__attribute__ ((cogmain, noreturn)) void __entry() {
    // basic entry code to jump to our resuable startup code. we do this by restarting cog 0, copying in
    // the code in the cog section (our reusable startup code). The linker will place _start() at address 0x100

    // this function might get overwritten later by hub params (clkfreq, clkmode, etc), so DO NOT try to restart the code with coginit #0, #0

    // before we start the routine, enable debugging for cog 0. any other cogs that want to be debugged should be enabled by the application
    //if (__enable_p2db) _hubset(DEBUG_INT_EN | DEBUG_COG0);
    asm("coginit #0, %0" : : "r"(__start));
}
#pragma clang diagnostic pop

void __start() {
    int r;

    // TODO: figure out how to rewrite this without needing inline asm.
    INIT_RTLIB;

    asm("cogid r0\n"           // get the current cog ID
        "tjz r0, #.Linit\n"    // if cog 0, jump to the special cog0 startup code.

        "mov r0, ptra\n"      //  if not cog 0, save ptra (value at ptra should be pointer to start of stack)
        "rdlong r1, r0\n"     //  read out first stack value
        "add r0, #4\n"
        "rdlong r0, r0\n"     //  read out second stack value
        "jmp r1\n"             //  jump to the cog function
        ".Linit:"
    );

    // initialize the stack
    PTRA = (unsigned int)&__stack;

    // init the debug lock
    _dbg_lock = _locknew();

    // default clock frequency. Assumes the loader does not try to set a different one.
    // if we want the loader to be able to configure it, we'll need to pull this number from some shared memory location
    _clkfreq = 200000000;

    asm("hubset #0\n"
        "augd #32772\n"
        "hubset #504\n"
        "augd #390\n"
        "waitx #140\n"
        "augs #32772\n"
        "mov pa, #507\n"
        "hubset pa\n"
        "wrlong pa, #24\n"
        "augd #390625\n"
        "wrlong #511, #20\n"
    );

    // setup c standard library
    _cstd_init();

    // initialize all our constructors
    _init();

    argv[0] = program;
    argv[1] = _dummy;

    // run the main function
    r = main(argc, argv);

    if (r == -1)
        asm("drvl #57\n"::);

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
    while(1) {
      _wait(1000);
    }
}

void __cxa_pure_virtual() {
    __unreachable();
}