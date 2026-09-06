#include "observe.h"
extern int local_class(int), default_call(int), static_member();
extern int *allocate(int);
extern void release(int *);
// A bounded allocator fixture checks new/delete lowering without requiring a
// particular C library heap implementation.
static unsigned storage[64];
static volatile unsigned deletes;
void *operator new[](decltype(sizeof(0)) n) { return n <= sizeof(storage) ? storage : nullptr; }
void operator delete[](void *) noexcept { ++deletes; }
void operator delete(void *) noexcept {}
void operator delete(void *, decltype(sizeof(0))) noexcept {}
extern "C" __attribute__((noreturn)) void abort(void) {
    observe("unexpected.abort", 1);
    for (;;) __asm__ volatile("");
}
extern "C" void test_body(void) {
    observe("cxx.virtual", local_class(29));
    observe("cxx.default", default_call(7));
    observe("cxx.static", static_member());
    int *p = allocate(4);
    p[0] = 19; p[3] = 23;
    observe("cxx.new", p[0] + p[3]);
    release(p);
    observe("cxx.delete_returned", deletes);
}
