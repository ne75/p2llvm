/**
 * return -a
 * 
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __negdi2(long long a) {
    asm(
        "not $r30, $r0\n"
        "not $r31, $r1\n"
        "add $r30, #1   wc\n"
        "addx $r31, #0\n"
    );
}
#pragma clang diagnostic pop