// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -S %s -o %t.O2.s
// RUN: FileCheck %s < %t.O2.s
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
// CHECK-LABEL: pair_constant:
// CHECK-NOT: calla
// CHECK: mov r30,
// CHECK: mov r31,
// CHECK-NOT: calla
// CHECK: reta
// CHECK-LABEL: pair_add:
// CHECK-NOT: calla
// CHECK: add r0, r2{{.*}}wc
// CHECK-NEXT: addx r1, r3
// CHECK-NOT: calla
// CHECK: reta

typedef unsigned long long u64;
u64 pair_constant(void) {
#ifdef P2_TEST_HOST
    return 0x89abcdef01234567ULL;
#else
    register u64 result __asm__("r30_r31");
    __asm__ volatile("mov %L0, ##0x01234567\nmov %H0, ##0x89abcdef"
                     : "=r"(result));
    return result;
#endif
}

u64 pair_add(u64 a, u64 b) {
#ifndef P2_TEST_HOST
    __asm__("add %L0, %L1 wc\naddx %H0, %H1" : "+&r"(a) : "r"(b) : "cc");
    return a;
#else
    return a + b;
#endif
}
