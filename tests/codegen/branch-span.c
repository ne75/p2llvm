// RUN: %clang -O2 -S %s -o %t.s
// RUN: FileCheck %s < %t.s
// RUN: %mc -filetype=obj %t.s -o %t.o
// CHECK-LABEL: branch_span:
// CHECK-NOT: tjz
// CHECK-NOT: tjnz
// CHECK: jmp
unsigned branch_span(unsigned x) {
    if (x) {
#ifndef P2_TEST_HOST
        __asm__ volatile(".rept 300\n nop\n .endr");
#endif
        return 7;
    }
    return 9;
}
