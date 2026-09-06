// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
struct Base { virtual int value() const = 0; virtual ~Base() {} };
struct Derived : Base { int x; Derived(int n) : x(n) {} int value() const override { return x; } };
int dispatch(const Base &x) { return x.value(); }
int local_class(int n) { Derived x(n); return dispatch(x); }
int default_argument(int x, int y = 3) { return x + y; }
int default_call(int x) { return default_argument(x); }
struct Static { static int x; };
int Static::x = 7;
int static_member() { return Static::x; }
int *allocate(int n) { return new int[n]; }
void release(int *p) { delete[] p; }
