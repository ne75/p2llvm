// RUN: %clang -O2 -c %s -o %t.direct.o
// RUN: %clang -O2 -S %s -o %t.s
// RUN: %mc -filetype=obj %t.s -o %t.assembly.o
// RUN: llvm-objcopy --only-section=.text -O binary %t.direct.o %t.direct.bin
// RUN: llvm-objcopy --only-section=.text -O binary %t.assembly.o %t.assembly.bin
// RUN: cmp %t.direct.bin %t.assembly.bin
// RUN: llvm-readobj -r %t.assembly.o | FileCheck %s
extern int g;
extern void use(int *);
int *address(void) { return &g; }
int local(int n) { int x = n; use(&x); return x; }
int branch(int a, int b) { if (a) return b + 7; use(&b); return b - 2; }
// CHECK: R_P2_AUG_HI23 g
// CHECK: R_P2_AUGS_LO9 g
// CHECK-NOT: $g
// CHECK-NOT: $.L
