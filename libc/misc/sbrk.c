/*
 * get memory from the OS
 * written by Eric R. Smith, placed in the public domain
 * FIXME: should do error checking based on the stack; but
 * that is tricky if threads alloc their memory from the stack
 * for now, use the stack of the first thread to request memory
 */

#include <compiler.h>

__attribute__ ((section (".heap"))) char __heap_start;

char *_heap_base = &__heap_start;

char *_sbrk(unsigned long n) {
    //char c;
    //char *here = &c;
    char *r = _heap_base;

    /* allocate and return */
    _heap_base = r + n;
    return r;
}

/* for porting C programs */
__weak_alias(sbrk, _sbrk);
