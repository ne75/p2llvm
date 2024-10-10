#include <propeller.h>
#include <sys/thread.h>
#include <driver.h>

// extern int malloc_lock;
// extern int __stdio_lock;

extern int __sys_lock;

// function to call to initialize necessary things for the C standard library (such as locks and default drivers, etc)
void _cstd_init() {
    // don't need these anymore--use system lock
    // malloc_lock = _locknew();
    // __stdio_lock = _locknew();

    _InitIO();
    // might need to init these somehwere?
    // dellock
}