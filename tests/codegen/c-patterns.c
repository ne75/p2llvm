// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
// Compile externally observable functions: optimization cannot fold the inputs.
typedef unsigned long long u64;
typedef long long i64;
struct pair { int x, y; };
struct __attribute__((packed)) packed { char x; int y; };
extern int external_call(int, int, int, int, int, int);
extern void escape(void *);
volatile int global;
volatile u64 global64;

int arithmetic(int a, int b) { return ((a + b) ^ (a - b)) & (a | b); }
unsigned shifts(unsigned a, unsigned n) { return (a << n) ^ (a >> n); }
i64 signed_shift(i64 a, unsigned n) { return a >> n; }
u64 wide(u64 a, u64 b) { return (a + b) ^ (a - b) ^ 0x123456789abcdef0ULL; }
int signed_less(i64 a, i64 b) { return a < b; }
int unsigned_less(u64 a, u64 b) { return a < b; }
unsigned quotient(unsigned a, unsigned b) { return a / b; }
unsigned remainder(unsigned a, unsigned b) { return a % b; }
int signed_quotient(int a, int b) { return a / b; }
int signed_remainder(int a, int b) { return a % b; }
unsigned clz_defined(unsigned x) { return x ? __builtin_clz(x) : 32; }
int multiply_overflow(int a, int b) { int result; return __builtin_mul_overflow(a, b, &result); }
int select_value(int a, int b, int t, int f) { return a < b ? t : f; }
int narrow(signed char a, unsigned short b) { return a + b; }
int array(int *p, unsigned n) { int result = 0; for (unsigned i = 0; i < n; ++i) result += p[i]; return result; }
int control(unsigned n) { switch (n) { case 0: return 4; case 3: return 7; case 8: return 1; default: return -1; } }
int stack_arguments(int a) { return external_call(a, a+1, a+2, a+3, a+4, a+5); }
int packed_argument(int a, int b, int c, int d, struct packed p, int e) { return p.y + e; }
struct pair aggregate_return(int a, int b) { struct pair p = {a, b}; return p; }
int indirect(int (*f)(int), int a) { return f(a); }
__attribute__((noinline)) int recursion(int n) { return n <= 0 ? 0 : global + recursion(n - 1); }
int large_frame(int n) { int a[150]; escape(a); a[140] = n; escape(a); return a[140]; }
u64 wide_memory(u64 *p, u64 a) { *p = a; global64 = *p; return global64; }
void global_memory(int n) { global = n; }
int *global_address(void) { return (int *)&global; }
