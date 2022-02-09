#include <stdlib.h>

long
labs(long n) {
  long x;
  asm("abs %0, %1" : "=r"(x) : "r"(n));
  return x;
}

#if defined(__GNUC__) && (_INT_SIZE == _LONG_SIZE)
int abs(int n) __attribute__((alias("labs")));
#else
int
abs(int n)
{
  return (n < 0) ? -n : n;
}
#endif
