// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
// RUN: %clang -O0 -S -emit-llvm %s -o - | FileCheck %s --check-prefix=ABI
// ABI: getelementptr inbounds i8, i8* {{.*}}, i32 -8
// ABI: getelementptr inbounds i8, i8* {{.*}}, i32 -5
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

struct __attribute__((packed)) five { char c; int i; };
__attribute__((noinline)) int mixed(int n, ...) {
    va_list args;
    __builtin_va_start(args, n);
    struct five p = __builtin_va_arg(args, struct five);
    int tail = __builtin_va_arg(args, int);
    __builtin_va_end(args);
    return n + p.c + p.i + tail;
}
int call_mixed(int a, int b) { struct five p = {7, a}; return mixed(3, p, b); }
__attribute__((noinline)) int stack_narrow(int a, int b, int c, int d,
                                          signed char e, unsigned short f) {
    return a + b + c + d + e + f;
}
int call_stack_narrow(void) { return stack_narrow(1, 2, 3, 4, -7, 65530); }
