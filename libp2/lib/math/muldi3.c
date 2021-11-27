/**
 * return product of signed a*b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __muldi3(long long a, long long b) {
    // let a = a0 + a1 << 32, same for b
    // a*b = a0*b0 + (a1*b0 + a0*b1) << 32 (the a1*b1 term gets shifted by 64 and is therefore always 0)
    // multiply the 3 parts, then get qx/qy for the first one, then get qx into the upper part of the result
    // to shift by 32 right away

    asm(
        // a0*b0
        "qmul $r0, $r2\n"
        "getqx $r30\n"
        "getqy $r31\n"

        // a1*b0
        "qmul $r1, $r2\n"
        "getqx $r3\n" // r3 = upper 32 of a1*b0 << 32
        
        // a0*b1
        "qmul $r0, $r3\n"
        "getqx $r4\n" // r4 = upper 32 of a0*b1 << 32

        // add up
        "add $r31, $r3\n"
        "add $r31, $r4\n"
    : : : "r0", "r1", "r2", "r3", "r4");
}
#pragma clang diagnostic pop