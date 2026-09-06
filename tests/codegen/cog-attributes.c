// RUN: %clang -O2 -emit-llvm -c %s -o %t.bc
// RUN: %bin/llvm-dis %t.bc -o %t.ll
// RUN: FileCheck %s < %t.ll
// RUN: %llc -filetype=obj %t.bc -o %t.o
// CHECK-DAG: "cogmain"
// CHECK-DAG: "cogtext"
// CHECK-DAG: "cogcache"
#ifndef P2_TEST_HOST
#define MAIN __attribute__((cogmain))
#define TEXT __attribute__((cogtext))
#define CACHE __attribute__((cogcache))
#else
#define MAIN
#define TEXT
#define CACHE
#endif
MAIN unsigned attr_entry(void) { return 37; }
TEXT unsigned attr_text(unsigned x) { return x + 7; }
CACHE unsigned attr_cache(unsigned x) { return x ^ 0x1234; }
