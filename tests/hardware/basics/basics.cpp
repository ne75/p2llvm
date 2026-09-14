#include "basics.h"
#define NI __attribute__((noinline))
struct Block { u32 a, b, c, d; };
NI static Block transform(Block value, u32 n) {
    return {value.d + n, value.a ^ n, value.b + 7, value.c * 3};
}
extern "C" NI u32 basic_aggregate(u32 n) {
    Block value = transform({n, n + 1, n + 2, n + 3}, n);
    return value.a ^ value.b ^ value.c ^ value.d;
}
struct Operation { virtual u32 run(u32) const = 0; };
struct Add : Operation { u32 run(u32 n) const override { return n + 17; } };
struct Multiply : Operation { u32 run(u32 n) const override { return n * 13; } };
static Add add;
static Multiply multiply;
NI static u32 dispatch(const Operation *operation, u32 n) { return operation->run(n); }
extern "C" NI u32 basic_virtual(u32 n) {
    const Operation *operation = n & 1 ? static_cast<const Operation*>(&add) : &multiply;
    return dispatch(operation, n);
}
