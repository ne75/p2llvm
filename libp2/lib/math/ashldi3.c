/**
 * return a << b
 * 
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __ashldi3(long long a, int b) {
    asm(
            "cmp $r2, #32   wc\n"
    "if_nc  jmp #.Lge32\n"
            // if n < 32
            "mov $r3, #32\n"
            "sub $r3, $r2\n"
            
            // hi = hi_in << n | (low_in >> (32-n))
            // lo = lo_in << n

            "mov $r30, $r0\n"
            "shl $r30, $r2\n"

            "mov $r31, $r1\n"
            "shl $r31, $r2\n"
            "shr $r0, $r3\n"
            "or $r31, $r0\n"

            "jmp #.Lret\n"

    ".Lge32:\n"
            // n >= 32
            "mov $r3, $r2\n"
            "sub $r3, #32\n"

            // hi = lo_in << (n-32)
            // lo = 0
            "mov $r30, #0\n"
            "mov $r31, $r0\n"
            "shl $r31, $r3\n"

    ".Lret:\n"
    : : : "r0", "r1", "r2", "r3");
}
#pragma clang diagnostic pop