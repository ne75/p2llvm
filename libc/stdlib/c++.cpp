/**
 * Basic implementations of desired C++ functionality
 */
#include <stdlib.h>

void *operator new (size_t sz) {
    void *p;

    /* malloc (0) is unpredictable; avoid it.  */
    if (sz == 0)
        sz = 1;

    p = (void *) malloc (sz);
    while (p == 0) {
        p = (void *) malloc (sz);
    }

    return p;
}

void* operator new[] (size_t sz) {
    return ::operator new(sz);
}

void operator delete(void* ptr) noexcept {
    if (ptr)
        free(ptr);
}

void operator delete[] (void *ptr) noexcept {
    ::operator delete (ptr);
}