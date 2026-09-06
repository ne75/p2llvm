// RUN: %clang -O0 -fno-builtin -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -fno-builtin -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -fno-builtin -mllvm -verify-machineinstrs -c %s -o %t.Os.o
typedef __SIZE_TYPE__ size_t;
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
unsigned memory_return;
static unsigned hash(const unsigned char *p, unsigned n) {
    unsigned h = 0;
    for (unsigned i = 0; i < n; ++i) h = h * 31 + p[i];
    return h;
}
__attribute__((no_builtin("memcpy", "memset")))
unsigned memory_copy(unsigned n) {
    unsigned char src[532], dst[532];
    for (unsigned i = 0; i < 532; ++i) src[i] = i*17 + 3;
    memory_return = memset(dst, 0xa5, 532) == dst;
    memory_return &= memcpy(dst+1, src+1, n) == dst+1;
    return hash(dst, 532);
}
__attribute__((no_builtin("memmove")))
unsigned memory_move(unsigned direction, unsigned n) {
    unsigned char p[12];
    for (unsigned i = 0; i < 12; ++i) p[i] = i+1;
    unsigned char *dst = p + (direction ? 2 : 0);
    unsigned char *src = p + (direction ? 0 : 2);
    memory_return = memmove(dst, src, n) == dst;
    return hash(p, 12);
}
__attribute__((no_builtin("memset")))
unsigned memory_set_zero(void) {
    unsigned char p[4] = {1, 2, 3, 4};
    memory_return = memset(p, 0xff, 0) == p;
    return hash(p, 4);
}
