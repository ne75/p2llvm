// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
extern unsigned inspect_alignment(void *, void *, unsigned);
unsigned aligned_locals(unsigned n) {
    _Alignas(16) unsigned char a[19];
    _Alignas(32) unsigned char b[35];
    a[0] = n; a[18] = n + 1; b[0] = n + 2; b[34] = n + 3;
    return inspect_alignment(a, b, n);
}
