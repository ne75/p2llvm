#include <propeller.h>
#include <sys/thread.h>
#include <driver.h>

extern atomic_t malloc_lock;
extern atomic_t __stdio_lock;

// function to call to initialize necessary things for the C standard library (such as locks and default drivers, etc)
void _cstd_init() {
    malloc_lock = _locknew();
    __stdio_lock = _locknew();

    _InitIO();
    // might need to init these somehwere?
    // dellock
}