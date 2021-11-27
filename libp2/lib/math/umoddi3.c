/**
 * return remainder of unsigned a/b
 */

extern unsigned long long __udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *r);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __umoddi3(unsigned long long a, unsigned long long b) {
    unsigned long long r;
    __udivmoddi4(a, b, &r);
    return r;
}
#pragma clang diagnostic pop
