/**
 * return logical shift right of a by n
 * 
 * a is stored in r0 and r1. n is in r2
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __lshrdi3(long long a, int n) {
    asm(
            "cmp $r2, #32   wc\n"
    "if_nc  jmp #.Lge32\n"
            // if n < 32
            "mov $r3, #32\n"
            "sub $r3, $r2\n"
            
            // hi = hi_in >> n
            // lo = lo_in >> n | (hi_in << (32-n))

            "mov $r31, $r1\n"
            "shr $r31, $r2\n"

            "mov $r30, $r0\n"
            "shr $r30, $r2\n"
            "shl $r1, $r3\n"
            "or $r30, $r1\n"

            "jmp #.Lret\n"

    ".Lge32:\n"
            // n >= 32
            "mov $r3, $r2\n"
            "sub $r3, #32\n"

            // hi = 0
            // lo = hi_in >> (n-32)
            "mov $r31, #0\n"
            "mov $r30, $r1\n"
            "shr $r30, $r3\n"

    ".Lret:\n"
    : : : "r0", "r1", "r2", "r3");
}
#pragma clang diagnostic pop