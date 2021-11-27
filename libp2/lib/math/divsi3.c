/**
 * return quotient of signed of a/b
 */ 
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) int __divsi3(int a, int b) {
    // r0 = a, r1 = b
    asm(
               "mov $r2, $r0\n"
               "xor $r2, $r1\n"
               "shr $r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
               "abs $r0, $r0\n"
               "abs $r1, $r1\n"
               "qdiv $r0, $r1\n"
               "getqx $r31\n"
        "if_nz  neg $r31, $r31\n"
    : : : "r0", "r1", "r2");   
}
#pragma clang diagnostic pop