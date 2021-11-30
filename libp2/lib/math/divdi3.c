/**
 * return quotient of signed a/b. 
 * 
 * If divisor is > 32 bits, precision is reduced by right-shifts until divisors is 32 bits
 * 
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __divdi3(long long a, long long b) {
    asm(    
            "mov $r4, $r1\n"
            "shr $r4, #31\n"    // sign of a
            "mov $r5, $r3\n"
            "shr $r5, #31\n"    // sign of b
            "mov $r6, $r4\n"
            "xor $r6, $r5\n"    // sign of result
            
            "cmp $r4, #0    wz\n"
    "if_z   jmp #.Lskip_neg_a\n"    // a is positive? 
            // a = -a;
            "not $r0, $r0\n"
            "not $r1, $r1\n"
            "add $r0, #1    wc\n"
            "addx $r1, #0\n"

    ".Lskip_neg_a:"

            "cmp $r5, #0    wz\n"
    "if_z   jmp #.Lskip_neg_b\n"    // b is positive?

            // b = -b;
            "not $r2, $r2\n"
            "not $r3, $r3\n"
            "add $r2, #1 wc\n"
            "addx $r3, #0\n"
    ".Lskip_neg_b:"

            "wrlong #0, $ptra\n"
            "add $ptra, #4\n"
            "calla #__udivmoddi4\n" // call unsigned division
            "sub $ptra, #4\n"

            "cmp $r6, #0    wz\n"
    "if_z   jmp #.Lskip_neg_r\n"    // is result positive? 

            "not $r30, $r30\n"
            "not $r31, $r31\n"
            "add $r30, #1   wc\n"
            "addx $r31, #0\n"

    ".Lskip_neg_r:"
    : : : "r0", "r1", "r2", "r3", "r4", "r5", "r6");
}
#pragma clang diagnostic pop