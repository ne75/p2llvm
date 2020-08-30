#include <propeller.h>
#include <sys/thread.h>

extern atomic_t malloc_lock;

// function to call to initialize necessary things for the C standard library (such as locks and default drivers, etc)
void _cstd_init() {
    malloc_lock = _locknew();
}