#include <errno.h>

/*
 * default thread local variable
 */
static _thread_state_t default_thread;
_thread_state_t *_TLS = &default_thread;

/*
 * default function for giving up CPU time
 */
static void
dummy(void)
{
}

void (*__yield_ptr)(void) = dummy;

/*
 * function for sleeping until a specific cycle is reached or passed
 */
void (*__napuntil_ptr)(unsigned int untilcycle) = 0;

