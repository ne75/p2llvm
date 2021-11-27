/**
 * return quotient of unsigned a/b.
 * 
 * if b is = 0, return the largest possible integer, 0xffffffff_ffffffff
 */
extern unsigned long long __udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *r);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) unsigned long long __udivdi3(unsigned long long a, unsigned long long b) {
    unsigned long long *r = 0;
    return __udivmoddi4(a, b, r);
}
#pragma clang diagnostic pop