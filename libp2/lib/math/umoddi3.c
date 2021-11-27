/**
 * return remainder of unsigned a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __umoddi3(unsigned long long a, unsigned long long b) {
    asm(
        ""
    );
}
#pragma clang diagnostic pop
