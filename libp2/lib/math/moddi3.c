/**
 * return remainder of signed a/b
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
__attribute__ ((section ("lut"), cogtext)) long long __moddi3(long long a, long long b) {
    long long r;
    asm(    
            "mov $r4, $r1\n"
            "shr $r4, #31\n"    // sign of a
            "mov $r5, $r3\n"
            "shr $r5, #31\n"    // sign of b
            "xor $r6, $r5\n"    // sign of result = sign of a (by C99/LLVM)
            
            "cmp $r4, #0    wz\n"
    "if_z   jmp #.Lskip_neg_a\n"    // a is positive? 
            // a = -a;
            "not $r0, $r0\n"
            "not $r1, $r1\n"
            "add $r0, #1\n"
            "addx $r1, #0\n"

    ".Lskip_neg_a:"

            "cmp $r5, #0    wz\n"
    "if_z   jmp #.Lskip_neg_b\n"    // b is positive?

            // b = -b;
            "not $r2, $r2\n"
            "not $r3, $r3\n"
            "add $r2, #1\n"
            "addx $r3, #0\n"
    ".Lskip_neg_b:"

            "wrlong %0, $ptra\n"    // manually push 1 argument to the stack (since this is the 5th long to the function)
            "add $ptra, #4\n"
            "calla #__udivmoddi4\n" // call unsigned division
            "sub $ptra, #4\n"
            "setq #1\n"
            "rdlong $r30, %0\n"

            "cmp $r6, #0    wz\n"
    "if_z   jmp #.Lskip_neg_r\n"    // is result positive? 

            "not $r30, $r30\n"
            "not $r31, $r31\n"
            "add $r30, #1\n"
            "addx $r31, #0\n"

    ".Lskip_neg_r:"
    : : "r"(&r) : "r0", "r1", "r2", "r3", "r4", "r5", "r6");
}
#pragma clang diagnostic pop