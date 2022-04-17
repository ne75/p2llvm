/**
 * All of the core code necessary to run our compiled binaries
 */

#include "propeller2.h"
#include "debug.h"

typedef void (*func_ptr)(void);

extern unsigned int __stack;

__attribute__ ((cogmain, noreturn)) void __entry();
__attribute__ ((section ("cog"), cogmain, noreturn)) void __start0();
__attribute__ ((section ("cog"), cogmain)) void __start();

// the relocation for these calls isn't encoded properly since the linker thinks the calls are global calls, not external "libcalls"
void _init();
void _fini();
void __start0();

extern int main();
extern void _cstd_init();
extern func_ptr _init_array_start[];
extern func_ptr _init_array_end[];
extern func_ptr _fini_array_start[];
extern func_ptr _fini_array_end[];
extern char _bss_start[];
extern char _bss_end[];
extern unsigned _libcall_start[];
extern int __enable_p2db;


// basic entry code to jump to our resuable startup code. we do this by restarting cog 0, copying in
// the code in the cog section (our reusable startup code). The linker will place _start0() at 0x40, so jump to 0x10 since this is in cog mode
// before we start the routine, enable debugging for cog 0. any other cogs that want to be debugged should be enabled by the application
// We split it into 2 functions so that their is a gap in memory for hub parameters (clkfreq, clkmode, etc)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
void __entry() {
    asm("jmp #\\0x10"); // TODO figure out how to not hardcode this address. requires solving how to call cog functions from other cog functions
}

void __start0() {
    if (__enable_p2db) hubset(DEBUG_INT_EN | DEBUG_COG_ALL);
    asm("coginit #0, %0" : : "r"(__start));
}
#pragma clang diagnostic pop

void __start() {
    INIT_RTLIB;

    asm("cogid r0\n"           // get the current cog ID
        "tjz r0, #.Linit\n"    // if cog 0, jump to the special cog0 startup code.

        "mov r0, ptra\n"      //  if not cog 0, save ptra (value at ptra should be pointer to start of stack)
        "rdlong r1, r0\n"     //  read out first stack value
        "add r0, #4\n"
        "rdlong r0, r0\n"     //  read out second stack value
        "jmp r1\n"             //  jump to the cog function
        ".Linit:"
    ::: "r0", "r1");

    // initialize the stack
    PTRA = (unsigned int)&__stack;

    // clear bss
    for (char *ptr = _bss_start; ptr < _bss_end; ptr++) {
        *ptr = 0;
    }

    // init the debug lock
    _dbg_lock = _locknew();

    // if settings weren't patched in, set to a default
    if (_clkfreq == 0) _clkfreq = 24000000;
    if (_dbgbaud == 0) _dbgbaud = DBG_UART_BAUD;

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