#include "observe.h"
typedef unsigned long long u64;
typedef long long i64;
struct pair { int x, y; };
struct __attribute__((packed)) packed { char x; int y; };
extern int arithmetic(int, int), signed_less(i64, i64), unsigned_less(u64, u64);
extern unsigned shifts(unsigned, unsigned), quotient(unsigned, unsigned);
extern unsigned remainder(unsigned, unsigned), clz_defined(unsigned);
extern i64 signed_shift(i64, unsigned);
extern u64 wide(u64, u64), wide_memory(u64 *, u64);
extern int signed_quotient(int, int), signed_remainder(int, int);
extern int multiply_overflow(int, int), select_value(int, int, int, int);
extern int narrow(signed char, unsigned short), array(int *, unsigned);
extern int control(unsigned), stack_arguments(int), recursion(int), large_frame(int);
extern int packed_argument(int, int, int, int, struct packed, int);
extern struct pair aggregate_return(int, int);
extern int indirect(int (*)(int), int);
extern void global_memory(int);
extern int *global_address(void);
extern volatile int global;
int external_call(int a, int b, int c, int d, int e, int f) {
    return a + 2*b + 3*c + 4*d + 5*e + 6*f;
}
void escape(void *p) { __asm__ volatile("" : : "r"(p) : "memory"); }
static int callback(int n) { return n + 19; }
void test_body(void) {
    observe("arithmetic", arithmetic(17, 5));
    observe("shifts", shifts(0x80000001u, 1));
    OBSERVE64("signed_shift", signed_shift(-0x100000002LL, 1));
    OBSERVE64("wide", wide(0x100000001ULL, 2));
    observe("signed64.low_sign", signed_less(0x80000000LL, 0));
    observe("signed64.negative", signed_less(-1, 0));
    observe("unsigned64.high", unsigned_less(0x8000000000000000ULL, 0));
    observe("quotient", quotient(100, 7));
    observe("remainder", remainder(100, 7));
    observe("signed_quotient", signed_quotient(-100, 7));
    observe("signed_remainder", signed_remainder(-100, 7));
    observe("clz.zero", clz_defined(0));
    observe("clz.one", clz_defined(1));
    observe("clz.top", clz_defined(0x80000000u));
    observe("smul.negative_no_overflow", multiply_overflow(-1, 1));
    observe("smul.positive_overflow", multiply_overflow(0x7fffffff, 2));
    observe("select.true", select_value(-4, 2, 19, 23));
    observe("select.false", select_value(4, 2, 19, 23));
    observe("narrow", narrow(-4, 65530));
    int a[] = {1, -2, 7, 9};
    observe("array", array(a, 4));
    observe("switch.hit", control(3));
    observe("switch.default", control(2));
    observe("stack.arguments", stack_arguments(2));
    struct packed p = {7, 0x12345678};
    observe("stack.packed", packed_argument(1, 2, 3, 4, p, 19));
    struct pair pair = aggregate_return(-17, 23);
    observe("aggregate.x", pair.x); observe("aggregate.y", pair.y);
    observe("indirect", indirect(callback, 4));
    global = 7; observe("recursion", recursion(4));
    observe("large_frame", large_frame(0x12345678));
    u64 memory;
    OBSERVE64("wide_memory", wide_memory(&memory, 0x123456789abcdef0ULL));
    global_memory(31); observe("global.value", global);
    observe("global.address", global_address() == (int *)&global);
}
