/**
 * return quotient of unsigned a/b, and the remainder of unsigned a/b in r
 * 
 * If divisor is > 32 bits, precision is reduced by right-shifts until divisors is 32 bits
 *
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) unsigned long long __udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *r) {
    asm(    
            // input validation/short circuits
            // 1. if b == 0, return max int 
            "cmp $r2, #0    wcz\n"
            "cmpx $r3, #0   wcz\n"
    "if_nz  jmp #.Lcheck_1\n"
            "augs #8388607\n"
            "mov $r4, #511\n"
            "mov $r30, $r4\n"
            "mov $r31, $r4\n"
            "jmp #.Lret\n"

            // 2. if b == 1, return a
    ".Lcheck_1:"
            "cmp $r2, #1    wcz\n"
            "cmpx $r3, #0   wcz\n"
    "if_nz  jmp #.Ldiv\n"
            "mov $r30, $r0\n"
            "mov $r31, $r1\n"
            "jmp #.Lret\n"
            
    ".Ldiv:"
            // first, reduce both a and b so that b fits in 32 bits
            "cmp $r3, #0    wz\n"
    "if_z   jmp #.Ldiv32\n"   // already fits in 32 bits, jump to division
            "encod $r4, $r3\n"  //
            "add $r4, #1\n"

            // shift a >> $r4
            "mov $r5, $r2\n"
            "mov $r2, $r4\n"
            "calla #__lshrdi3\n"
            "mov $r6, $r30\n"
            "mov $r7, $r31\n"

            // shift b >> $r4
            "mov $r0, $r5\n"
            "mov $r1, $r3\n"
            "calla #__lshrdi3\n"
            "mov $r2, $r30\n"
            "mov $r3, $r31\n"
            "mov $r0, $r6\n"
            "mov $r1, $r7\n"

    ".Ldiv32:"
            "cmp $r2, $r1  wc\n"
    "if_nc_and_nz  mov $r31, #0\n"
    "if_nc_and_nz  jmp #.Ldiv32final\n" // result will fit in 32 bits, skip getting the upper word
            "qdiv $r1, $r2\n"
            "getqx $r31\n"
            "getqy $r1\n"

    ".Ldiv32final:"
            "setq $r1\n"
            "qdiv $r0, $r2\n"
            "getqx $r30\n"

            "cmp %0, #0     wz\n"
    "if_z   jmp #.Lret\n"
             // if c is given, write the remainder
            "getqy $r0\n"
            "mov $r1, #0\n"
            "setq #1\n"       
            "wrlong $r0, %0\n"
            
    ".Lret:\n"
    : : "r"(r) : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7");
}
#pragma clang diagnostic pop