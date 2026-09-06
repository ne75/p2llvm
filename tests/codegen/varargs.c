// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
typedef __builtin_va_list va_list;
struct pair { int a, b; };
__attribute__((noinline)) int sum(int n, ...) {
    va_list args;
    __builtin_va_start(args, n);
    int result = 0;
    for (int i = 0; i < n; ++i) result += __builtin_va_arg(args, int);
    __builtin_va_end(args);
    return result;
}
__attribute__((noinline)) int aggregate(int n, ...) {
    va_list args;
    __builtin_va_start(args, n);
    struct pair p = __builtin_va_arg(args, struct pair);
    __builtin_va_end(args);
    return n + p.a + p.b;
}
int call_sum(int a, int b) { return sum(2, a, b); }
int call_aggregate(int a, int b) { struct pair p = {a, b}; return aggregate(1, p); }
