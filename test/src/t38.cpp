#include <stdio.h>
#include <stdint.h>
#include <debug.h>
#include <math.h>

#include <stdarg.h>

union float_rep {
    float f;
    unsigned int i;
};

__attribute__ ((noinline)) float __mulsf3_fast(float a, float b) { 
    /*
     * How to multiply 2 floats:
     * 1. break up the float into it's 3 parts: sign, fraction, exp
     * 2. xor the signs to get the new signs
     * 3. insert the implicit 1 to each mantissa by or'ing in 1 << 23
     * 4. multiply the mantissas together. 
     *      - the result is 47 or 48 bits. shift back down to 24 bits (so shift right by either 22 or 23 bits)
     *      - when we have 24 bits, add 1 to the resulting exponent
     *      - place the lower 23 of the result into the mantissa of the result
     * 
     * there's also a bunch of special code to handle when either the inputs or result
     * is subnormal/denormal
     * 
     * early return rules:
     * NaN * anything = NaN
     * infinity * non-zero = infinity w/ correct sign
     * infinity * zero = NaN
     * zero * non-infinity = 0
     * 
     * This implementation truncates and always sets signaling bits (resulting m == 1)
     * 
     * It's written with a mix of inline assembly and normal C
     * the inline asm is to optimize for things the compiler isn't great 
     * at optimizing yet. 
     */

    float_rep a_r = {.f = a};
    float_rep b_r = {.f = b};
    float_rep r_r = {.i = 0};

    int e1;
    int e2;
    int m1;
    int m2;
    int m1_frac;
    int m2_frac;
    int e1_denorm;
    int e2_denorm;
    int a_NaN;
    int a_Inf;
    int a_Zero;
    int b_NaN;
    int b_Inf;
    int b_Zero;
    int fraction_mask;
    int leading_one;
    int shift_hi;
    int shift_lo;
    int shift_extra;
    int top;
    int x;
    int y;
    int e;

    int exp_bits = 0xff << 23;
    int nan_bits = exp_bits + 1;

    asm volatile(
         // extract a exponent into e1. e = (x >> 23) & 0xff
        "mov %[e1], %[a]\n"
        "shr %[e1], #23\n"
        "and %[e1], #0xff\n"
        
        // extract b exponent into e2. 
        "mov %[e2], %[b]\n"
        "shr %[e2], #23\n"
        "and %[e2], #0xff\n"

        "bmask %[fraction_mask], #22\n" // fraction mask = 0x7fffff
        "decod %[leading_one], #23\n" // leading 1 bit = 1 << 23

        // extract fractional part for m1. m = x & fraction mask
        "mov %[m1], %[a]\n" 
        "and %[m1], %[fraction_mask]\n"
        "mov %[m1_frac], %[m1]\n"

        // extract fractional part for m2
        "mov %[m2], %[b]\n"
        "and %[m2], %[fraction_mask]\n"
        "mov %[m2_frac], %[m2]\n"

        // check for denorm
        // if exponent 1 is not 0, 'or' in the leading 1 else add 1 to the exponent to get -126 instead of -127
        "add %[e1], #0 wz\n"
        "if_nz or %[m1], %[leading_one]\n"
        "if_z add %[e1], #1\n"
        "wrz %[e1_denorm]\n"
        // if exponent 2 is not 0, 'or' in the leading 1
        "add %[e2], #0 wz\n"
        "if_nz or %[m2], %[leading_one]\n"
        "if_z add %[e2], #1\n"
        "wrz %[e2_denorm]\n"

        // start multiplying the mantissas 
        "qmul %[m1], %[m2]\n"

        // while the multiplication happens, compute the new sign and exponent
        "mov %[result], %[a]\n"
        "xor %[result], %[b]\n"
        "and %[result], #0x80000000\n" // r_r.i = 1<<31

        // initial exponent result
        "mov %[e], %[e1]\n"
        "add %[e], %[e2]\n"
        "sub %[e], #127\n"

        // Extract properties for each value: zero, nan, or infinity
        // TODO: try to make this faster.
        // r10 is scratch regs
        "sub %[e1], #255 wz\n" // e -= 255, write z 
        "wrz %[a_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[a_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m1_frac], #0 wz\n"   // m += 0, write z  
        "wrz r10\n" // if m == 0, r10 = 1 (else r10 = 0)   
        "wrz %[a_Zero]\n"
                           
        "andn %[a_NaN], r10\n" // Nan = e == 255 and !m == 0
        "and %[a_Inf], r10\n" // Inf = e == 255 and m == 0   
        "and %[a_Zero], %[e1_denorm]\n" // Zero = e = 0 and m = 0. We use the denorm flag and m before adding the leading 1

        // Extract zero, nan, or infinity for B
        "sub %[e2], #255 wz\n" // %[a_Zero] -= 255, write z 
        "wrz %[b_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[b_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m2_frac], #0 wz\n"   // m += 0, write z
        "wrz r10\n" // if m == 0, r10 = 1 (else r10 = 0)
        "wrz %[b_Zero]\n"
                           
        "andn %[b_NaN], r10\n"   
        "and %[b_Inf], r10\n"    
        "and %[b_Zero], %[e2_denorm]\n" 

         // make sure to set up for getqx/getqy BEFORE the 54 clocks runs out. otherwise, another cog that
        // has queued a cordic command might get this result instead of it's own or that cog's
        // result overwrites our own
        "getqx %[x]\n"
        "getqy %[y]\n" 

        // If either number is NaN, return NaN
        "mov r10, %[a_NaN]\n"
        "or r10, %[b_NaN] wz\n"
        "if_nz or %[result], %[nan_bits]\n" // 0xff << 23, +1 
        "if_nz jmp #.Lret_mulf32\n"

        // If A is inf: if B is 0, return NaN, else return Inf
        "add %[a_Inf], #0 wz\n"
        "if_nz or %[result], %[exp_bits]\n" // 0xff << 23
        "if_nz add %[result], %[b_Zero]\n" 
        "if_nz jmp #.Lret_mulf32\n"

        // If B is inf: if A is 0, return NaN, else return Inf
        "add %[b_Inf], #0 wz\n"
        "if_nz or %[result], %[exp_bits]\n" // 0xff << 23
        "if_nz add %[result], %[a_Zero]\n" 
        "if_nz jmp #.Lret_mulf32\n"

        // If a, b or both are zero, return zero
        "mov r10, %[a_Zero]\n"
        "or r10, %[b_Zero] wz\n"
        "tjnz r10, #.Lret_mulf32\n"

        // if both bumbers are denormal, return +/-0
        "mov r10, %[e1_denorm]\n"
        "and r10, %[e2_denorm] wz\n"
        "tjnz r10, #.Lret_mulf32\n"

        // we need to check if y is 0 (wc and if_nc) in which case we do encode 
        // on x
        "encod %[top], %[y] wc\n" // get thet top bit in y 
        "if_c sub %[top], #14\n"  // 14 is the bit position of the decimal place after the multiply
        "if_nc encod %[top], %[x]\n"
        "if_nc sub %[top], #46\n"

        // check for underflow
        "mov %[shift_extra], #0\n"
        "mov %[leading_one], %[e]\n" // use `leading_one` as a scratch register
        "add %[leading_one], %[top]\n"
        "cmps %[leading_one], #0 wcz\n"
        "if_c_or_z jmp #.Lunderflow_mulf32\n"

        // normal path
        "add %[e], %[top]\n" // adjust exponent for this shift
        "mov %[shift_hi], #9\n"
        "sub %[shift_hi], %[top]\n" // high word shift is 9 - top bit
        "mov %[shift_lo], #23\n" 
        "add %[shift_lo], %[top]\n" // low word shift is 23 + top bit

        "jmp #.Lunderflow_end_mulf32\n"

        // underflow path
        ".Lunderflow_mulf32:\n"
        "mov %[shift_extra], #1\n"
        "sub %[shift_extra], %[e]\n" // extra shift = 1 - e
        "cmp %[shift_extra], #24 wcz\n"
        "if_nc jmp #.Lret_mulf32\n" // we'll shift more than 24 bits, return 0

        "add %[e], %[shift_extra]\n"
        "sub %[e], #1\n"
        "mov %[shift_hi], #9\n"
        "mov %[shift_lo], #23\n"

        ".Lunderflow_end_mulf32:\n"

        "cmp %[e], #0xff wcz\n"
        "if_nc or %[result], %[exp_bits]\n"
        "if_nc jmp #.Lret_mulf32\n" // overflow

        // do the shift to get the 48 bit multiplication
        // back to 24 bits 
        "shr %[x], %[shift_lo]\n" // shift the low word
        "shl %[y], %[shift_hi]\n" // shift the high word
        "or %[x], %[y]\n" // or the high word to the low word
        "shr %[x], %[shift_extra]\n" // shif tthe extra bits

        // put the results in the output register
        "shl %[e], #23\n"
        "or %[result], %[e]\n"
        "and %[x], %[fraction_mask]\n"
        "or %[result], %[x]\n"

        ".Lret_mulf32:\n" // jump location for early returns

        : [result]"+r"(r_r.i), 
        [a]"+r"(a_r.i), 
        [b]"+r"(b_r.i), 
        [e1]"+r"(e1), 
        [e2]"+r"(e2), 
        [m1]"+r"(m1), 
        [m2]"+r"(m2),
        [fraction_mask]"+r"(fraction_mask), 
        [leading_one]"+r"(leading_one), 
        [m1_frac]"+r"(m1_frac), 
        [m2_frac]"+r"(m2_frac),
        [e1_denorm]"+r"(e1_denorm), 
        [e2_denorm]"+r"(e2_denorm), 
        [a_NaN]"+r"(a_NaN),
        [a_Inf]"+r"(a_Inf),
        [a_Zero]"+r"(a_Zero), 
        [b_NaN]"+r"(b_NaN), 
        [b_Inf]"+r"(b_Inf), 
        [b_Zero]"+r"(b_Zero),
        [e]"+r"(e),
        [x]"+r"(x),
        [y]"+r"(y),
        [top]"+r"(top),
        [shift_hi]"+r"(shift_hi),
        [shift_lo]"+r"(shift_lo),
        [shift_extra]"+r"(shift_extra),
        [exp_bits]"+r"(exp_bits),
        [nan_bits]"+r"(nan_bits)
        :: "r10", "r11"
    );

    // printf("%d\n", shift_extra);

    return r_r.f;
}

__attribute__ ((noinline)) float __divsf3_fast(float a, float b) {
    /*
     * Division is basically the same as multiplication, but need to subtract
     * the exponents and divide the mantissa
     * 
     * The final shifting needs to be the reverse of how it's done for multiplication:
     * Shift left to get the top 1 up to bit 23, unless the shift necessary is greater than
     * the exponent difference, then shift only by the size of the exponent and the result
     * is subnormal
     * 
     * Early return rules:
     * if either is NaN, return NaN
     * if both are 0, return NaN
     * if both are inf, return NaN
     * if b == 0, return +/-inf
     * if b == inf, return 0
     * if a == infinity and b is finite non zero, return infinity
     * 
     * So in order:
     * check either NaN, return NaN
     * if a is 0, check b. if b is 0, return NaN, else return 0
     * if b is 0, return inf. 
     * if a is inf, check b, if b is inf, return NaN, else return inf
     * if b is inf, return 0
     * 
     */

    float_rep a_r = {.f = a};
    float_rep b_r = {.f = b};
    float_rep r_r = {.i = 0};

    int e1;
    int e2;
    int m1;
    int m2;
    int m1_frac;
    int m2_frac;
    int e1_denorm;
    int e2_denorm;
    int a_NaN;
    int a_Inf;
    int a_Zero;
    int b_NaN;
    int b_Inf;
    int b_Zero;
    int fraction_mask;
    int leading_one;
    int shift_hi;
    int shift_lo;
    int shift_extra;
    int top;
    int x;
    int y;
    int e;

    int exp_bits = 0xff << 23;
    int nan_bits = exp_bits + 1;

    asm volatile(
         // extract a exponent into e1. e = (x >> 23) & 0xff
        "mov %[e1], %[a]\n"
        "shr %[e1], #23\n"
        "and %[e1], #0xff\n"
        
        // extract b exponent into e2. 
        "mov %[e2], %[b]\n"
        "shr %[e2], #23\n"
        "and %[e2], #0xff\n"

        "bmask %[fraction_mask], #22\n" // fraction mask = 0x7fffff
        "decod %[leading_one], #23\n" // leading 1 bit = 1 << 23

        // extract fractional part for m1. m = x & fraction mask
        "mov %[m1], %[a]\n" 
        "and %[m1], %[fraction_mask]\n"
        "mov %[m1_frac], %[m1]\n"

        // extract fractional part for m2
        "mov %[m2], %[b]\n"
        "and %[m2], %[fraction_mask]\n"
        "mov %[m2_frac], %[m2]\n"

        // check for denorm
        // if exponent 1 is not 0, 'or' in the leading 1 else add 1 to the exponent to get -126 instead of -127
        "add %[e1], #0 wz\n"
        "if_nz or %[m1], %[leading_one]\n"
        "if_z add %[e1], #1\n"
        "wrz %[e1_denorm]\n"
        // if exponent 2 is not 0, 'or' in the leading 1
        "add %[e2], #0 wz\n"
        "if_nz or %[m2], %[leading_one]\n"
        "if_z add %[e2], #1\n"
        "wrz %[e2_denorm]\n"

        // compute the new sign and exponent
        "mov %[result], %[a]\n"
        "xor %[result], %[b]\n"
        "and %[result], #0x80000000\n" // r_r.i = 1<<31

        // initial exponent result
        "mov %[e], %[e1]\n"
        "sub %[e], %[e2]\n"
        "add %[e], #127\n"

        // start dividing the mantissas using q frac. use the 64 bit result.
        // this result is extra 32 bits, so we'll need to adjust our exponent accordingly
        "qdiv %[m1], %[m2]\n"

        // Extract properties for each value: zero, nan, or infinity
        // r10 is scratch regs
        "sub %[e1], #255 wz\n" // e -= 255, write z 
        "wrz %[a_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[a_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m1_frac], #0 wz\n"   // m += 0, write z  
        "wrz r10\n" // if m == 0, r10 = 1 (else r10 = 0)   
        "wrz %[a_Zero]\n"
                           
        "andn %[a_NaN], r10\n" // Nan = e == 255 and !m == 0
        "and %[a_Inf], r10\n" // Inf = e == 255 and m == 0   
        "and %[a_Zero], %[e1_denorm]\n" // Zero = e = 0 and m = 0. We use the denorm flag and m before adding the leading 1

        // Extract zero, nan, or infinity for B
        "sub %[e2], #255 wz\n" // %[a_Zero] -= 255, write z 
        "wrz %[b_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[b_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m2_frac], #0 wz\n"   // m += 0, write z
        "wrz r10\n" // if m == 0, r10 = 1 (else r10 = 0)
        "wrz %[b_Zero]\n"
                           
        "andn %[b_NaN], r10\n"   
        "and %[b_Inf], r10\n"    
        "and %[b_Zero], %[e2_denorm]\n" 

        // make sure to set up for getqx BEFORE the 54 clocks runs out. otherwise, another cog that
        // has queued a cordic command might get this result instead of it's own or that cog's
        // result overwrites our own
        // x = quotient, y is remainder.
        // after we get the remainder, we need to do a 2nd division of the remainder by the divisor
        // using qfrac. 
        "getqx %[x]\n"
        "getqy %[y]\n"
        "qfrac %[y], %[m2]\n"

        "mov %[e1], %[x]\n"

        // If either number is NaN, return NaN
        "mov r10, %[a_NaN]\n"
        "or r10, %[b_NaN] wz\n"
        "if_nz or %[result], %[nan_bits]\n" // 0xff << 23, +1 
        // for any of these early returns, we need to make sure to grab the x values 
        // to flush the CORDIC so that another call to getqx/y doesn't grab these values
        // it's okay to not grab the Y value because all calls from the compiler will call getqx first, 
        // which will wait until the the X value is ready which will override the y value. 
        // "if_nz getqx %[x]\n"
        "if_nz jmp #.Lret_divf32\n"

        // If a is 0: if B is 0, return NaN, else return 0
        "test %[a_Zero], %[a_Zero] wc\n" // c = 1 if a_zero
        "mov r10, %[a_Zero]\n"
        "and r10, %[b_Zero] wz\n" // z = 0 if a_zero & b_zero
        "if_c_or_nz getqx %[x]\n"
        "if_nz or %[result], %[nan_bits]\n" // 0xff << 23, + 1
        "if_c_or_nz jmp #.Lret_divf32\n"

        // if b is 0, return inf (a == 0 case already handled)
        "add %[b_Zero], #0 wz\n"
        "if_nz or %[result], %[exp_bits]\n"
        "if_nz jmp #.Lret_divf32\n"

        // If a is inf: if B is inf, return NaN, else return inf
        "add %[a_Inf], #0, wz\n" // z = 1 if !a_inf
        "if_nz or %[result], %[exp_bits]\n" // 0xff << 23
        "if_nz add %[result], %[b_Inf]\n" // make result NaN
        "if_nz getqx %[x]\n"
        "if_nz jmp #.Lret_divf32\n"

        // if b is inf, return inf (a == inf case already handled)
        "add %[b_Inf], #0, wz\n"
        "if_nz or %[result], %[exp_bits]\n"
        "if_nz jmp #.Lret_divf32\n"

        "getqx %[y]\n" 
        "mov %[e2], %[y]\n"

        "encod %[top], %[x] wc\n" // get thet top bit in x
        "if_nc sub %[top], #32\n"
        "if_nc sub %[top], #1\n" // since top needs to be either 0 or -1, and encod will return 0 for both bit 0 = 1 and for S = 0

        // check for underflow
        "mov %[shift_extra], #0\n"
        "mov %[leading_one], %[e]\n"
        "add %[leading_one], %[top]\n" 
        "cmps %[leading_one], #0 wcz\n"
        "if_c_or_z jmp #.Lunderflow_divf32\n"

        // normal path
        "add %[e], %[top]\n" // adjust exponent for this shift
        "mov %[shift_hi], #23\n"
        "sub %[shift_hi], %[top]\n" // high word (quotient) shift is top
        "mov %[shift_lo], #9\n" 
        "add %[shift_lo], %[top]\n" // low word (remainder) shift is 9 + top bit

        "jmp #.Lunderflow_end_divf32\n"

        // underflow path
        ".Lunderflow_divf32:\n"
        "mov %[shift_extra], #1\n"
        "sub %[shift_extra], %[e]\n" // extra shift = 1 - e
        "cmp %[shift_extra], #24 wcz\n"
        "if_nc jmp #.Lret_divf32\n" // we'll shift more than 24 bits, return 0

        "add %[e], %[shift_extra]\n"
        "sub %[e], #1\n"
        "mov %[shift_hi], #23\n"
        "mov %[shift_lo], #9\n"

        ".Lunderflow_end_divf32:\n"

        "cmp %[e], #0xff wcz\n"
        "if_nc or %[result], %[exp_bits]\n"
        "if_nc jmp #.Lret_divf32\n" // overflow

        // do the shift to get the 48 bit multiplication
        // back to 24 bits 
        "shl %[x], %[shift_hi]\n" // shift the high word
        "shr %[y], %[shift_lo]\n" // shift the low word
        "or %[x], %[y]\n" // or the high word to the low word
        "shr %[x], %[shift_extra]\n" // shift the extra bits

        // put the results in the output register
        "shl %[e], #23\n"
        "or %[result], %[e]\n"
        "and %[x], %[fraction_mask]\n"
        "or %[result], %[x]\n"

        ".Lret_divf32:\n" // jump location for early returns

        : [result]"+r"(r_r.i), 
        [a]"+r"(a_r.i), 
        [b]"+r"(b_r.i), 
        [e1]"+r"(e1), 
        [e2]"+r"(e2), 
        [m1]"+r"(m1), 
        [m2]"+r"(m2),
        [fraction_mask]"+r"(fraction_mask), 
        [leading_one]"+r"(leading_one), 
        [m1_frac]"+r"(m1_frac), 
        [m2_frac]"+r"(m2_frac),
        [e1_denorm]"+r"(e1_denorm), 
        [e2_denorm]"+r"(e2_denorm), 
        [a_NaN]"+r"(a_NaN),
        [a_Inf]"+r"(a_Inf),
        [a_Zero]"+r"(a_Zero), 
        [b_NaN]"+r"(b_NaN), 
        [b_Inf]"+r"(b_Inf), 
        [b_Zero]"+r"(b_Zero),
        [e]"+r"(e),
        [x]"+r"(x),
        [y]"+r"(y),
        [top]"+r"(top),
        [shift_hi]"+r"(shift_hi),
        [shift_lo]"+r"(shift_lo),
        [shift_extra]"+r"(shift_extra),
        [exp_bits]"+r"(exp_bits),
        [nan_bits]"+r"(nan_bits)
        :: "r10", "r11"
    );

    // printf("%x %x\n", shift_hi, shift_lo);
    // printf("%x\n", m1);

    return r_r.f;
}

__attribute__ ((noinline)) float __addsf3_fast(float a, float b) {
    /**
     * how to add 2 floats
     * 
     * - check for NaN of Inf. if Inf, return Inf, if NaN, return NaN
     * - check for 0, return the other.
     * - a must be larger than b. if b > a, swap them. 
     * - the initial exponents is the exponent of a
     * - get the difference of the two exponents
     * - shift the mantissa of b by this difference
     * - add the two mantiassas based on the signs
     * - shift the result and adjust the exponent to normalize the result
     * - check for overflow/underflow
     */

    float_rep a_r = {.f = a};
    float_rep b_r = {.f = b};
    float_rep r_r = {.i = 0};

    // MANUAL REGISTER ALLOCATION
    // r0 = e1, later final exponent
    // r1 = e2
    // r2 = m1, later final mantissa
    // %[m2] = m2
    // %[m1_frac] = m1_frac
    // %[m2_frac] = m2_frac
    // %[e1_denorm] = e1_denorm
    // %[e2_denorm] = e2_denorm
    // %[a_NaN] = a_NaN
    // %[a_Inf] = a_inf
    // %[a_Zero] = a_zero
    // %[b_NaN] = b_nan
    // %[b_Inf] = b_inf
    // %[b_Zero] = b_zero
    // r14 = fraction mask
    // r15 = leading one
    // r20 = e result
    // remaining regs are scratch

    int e1;
    int e2;
    int m1;
    int m2;
    int m1_frac;
    int m2_frac;
    int e1_denorm;
    int e2_denorm;
    int a_NaN;
    int a_Inf;
    int a_Zero;
    int b_NaN;
    int b_Inf;
    int b_Zero;
    int fraction_mask;
    int leading_one;
    int e;
    int sign_a;
    int sign_b;
    int e_diff;

    int exp_bits = 0xff << 23;
    int nan_bits = exp_bits + 1;

    asm volatile(
        // extract a exponent into e1
        "mov %[e1], %[a]\n"
        "shr %[e1], #23\n"
        "and %[e1], #0xff\n"
        
        // extract b exponent into e2
        "mov %[e2], %[b]\n"
        "shr %[e2], #23\n"
        "and %[e2], #0xff\n"

        // compare e1 and e2. if e2 > e1, swap them, then swap the original values
        "cmp %[e2], %[e1] wcz\n"
        "if_nc_and_nz mov %[fraction_mask], %[e2]\n"
        "if_nc_and_nz mov %[e2], %[e1]\n"
        "if_nc_and_nz mov %[e1], %[fraction_mask]\n"

        "if_nc_and_nz mov %[fraction_mask], %[b]\n"
        "if_nc_and_nz mov %[b], %[a]\n"
        "if_nc_and_nz mov %[a], %[fraction_mask]\n"

        "bmask %[fraction_mask], #22\n" // fraction mask
        "decod %[leading_one], #23\n" // leading 1 bit

        // extract fractional part for m1
        "mov %[m1], %[a]\n"
        "and %[m1], %[fraction_mask]\n"
        "mov %[m1_frac], %[m1]\n"

        // extract fractional part for m2
        "mov %[m2], %[b]\n"
        "and %[m2], %[fraction_mask]\n"
        "mov %[m2_frac], %[m2]\n"

        // check for denorm
        // if exponent 1 is not 0, 'or' in the leading 1 else add 1 to the exponent to get -126 instead of -127
        "add %[e1], #0 wz\n"
        "if_nz or %[m1], %[leading_one]\n"
        "if_z add %[e1], #1\n"
        "wrz %[e1_denorm]\n"
        // if exponent 2 is not 0, 'or' in the leading 1
        "add %[e2], #0 wz\n"
        "if_nz or %[m2], %[leading_one]\n"
        "if_z add %[e2], #1\n"
        "wrz %[e2_denorm]\n"

        // Extract zero, nan, or infinity for A
        "mov r10, %[e1]\n"
                       
        "sub r10, #255 wz\n" // e -= 255, write z 
        "wrz %[a_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[a_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m1_frac], #0 wz\n"   // m += 0, write z  
        "wrz r11\n" // if m == 0, r11 = 1 (else r11 = 0)   
        "wrz %[a_Zero]\n"
                           
        "andn %[a_NaN], r11\n" // Nan = e == 255 and !m == 0
        "and %[a_Inf], r11\n" // Inf = e == 255 and m == 0   
        "and %[a_Zero], %[e1_denorm]\n" // Zero = e = 0 and m = 0. We use the denorm flag and m before adding the leading 1

        // Extract zero, nan, or infinity for B
        "mov r10, %[e2]\n" 
                       
        "sub r10, #255 wz\n" // %[a_Zero] -= 255, write z 
        "wrz %[b_NaN]\n"  // if e == 255, NaN = 1
        "wrz %[b_Inf]\n"  // if e == 255, Inf = 1
                       
        "add %[m2_frac], #0 wz\n"   // m += 0, write z
        "wrz r11\n" // if m == 0, r11 = 1 (else r11 = 0)
        "wrz %[b_Zero]\n"
                           
        "andn %[b_NaN], r11\n"   
        "and %[b_Inf], r11\n"    
        "and %[b_Zero], %[e2_denorm]\n" 

        "mov %[sign_a], %[a]\n"
        "shr %[sign_a], #31\n" // the sign of A
        "mov %[sign_b], %[b]\n"
        "shr %[sign_b], #31\n" // the sign of B

        // A or B is NaN, return NaN
        "mov r10, %[a_NaN]\n"
        "or r10, %[b_NaN] wz\n" 
        "if_nz or %[result], #0x7f800001\n" // 0xff << 23 + 1
        "tjnz r10, #.Lret_addf32\n"

        // A is +Inf
        "mov r10, %[a_Inf]\n"
        "andn r10, %[sign_a] wz\n"
        "mov r10, %[b_Inf]\n"
        "and r10, %[sign_b]\n"
        "if_nz or %[result], #0x7f800000\n" // set exponent bits
        "if_nz add %[result], r10\n" // B is -Inf so make the result NaN instead of Inf
        "if_nz jmp #.Lret_addf32\n"

        // A is -Inf
        "mov r10, %[a_Inf]\n"
        "and r10, %[sign_a] wz\n"
        "mov r10, %[b_Inf]\n"
        "andn r10, %[sign_b]\n"
        "if_nz or %[result], #0xff800000\n" // set exponent bits
        "if_nz add %[result], r10\n" // B is +Inf so make the result NaN instead of Inf
        "if_nz jmp #.Lret_addf32\n"
        
        // B is +Inf
        "mov r10, %[b_Inf]\n"
        "andn r10, %[sign_b] wz\n"
        "mov r10, %[a_Inf]\n"
        "and r10, %[sign_a]\n"
        "if_nz or %[result], #0x7f800000\n" // set exponent bits
        "if_nz add %[result], r10\n" // A is -Inf so make the result NaN instead of +Inf
        "if_nz jmp #.Lret_addf32\n"

        // B is -Inf
        "mov r10, %[b_Inf]\n"
        "and r10, %[sign_b] wz\n"
        "mov r10, %[a_Inf]\n"
        "andn r10, %[sign_a]\n"
        "if_nz or %[result], #0xff800000\n" // set exponent bits
        "if_nz add %[result], r10\n" // A is -Inf so make the result NaN instead of +Inf
        "if_nz jmp #.Lret_addf32\n"

        // A is 0 and B is 0
        "and %[a_Zero], %[b_Zero] wz\n"
        "if_nz mov %[result], %[sign_a]\n"
        "if_nz and %[result], %[sign_b]\n"
        "if_nz shl %[result], #31\n"
        "if_nz jmp #.Lret_addf32\n"

        // adjust m2 so that it's exponent matches m1
        // first shift up by 7 so that we can keep some of the precision before the shift.
        "shl %[m1], #6\n"
        "shl %[m2], #6\n"
        "mov %[e_diff], %[e1]\n"
        "sub %[e_diff], %[e2]\n"
        
        // if we don't enough bits to do this math and the result would just end up 
        // subtracting 0, so set m2 to 0. trying to shift by > 32 doesn't work in p2asm
        "cmp %[e_diff], #30 wcz\n"
        "if_nc mov %[m2], #0\n"
        "if_c shr %[m2], %[e_diff]\n"

        // if bit 31 is set (a is negative), negate m1
        "add %[sign_a], #0 wz\n"
        "if_nz neg %[m1], %[m1]\n"

        // if bit 31 is set (a is negative), negate m2
        "add %[sign_b], #0 wz\n"
        "if_nz neg %[m2], %[m2]\n"

        // compute the new mantissa with the correct sign
        "add %[m1], %[m2] wz\n"
        "tjz %[m1], #.Lret_addf32\n" // we subtracted to exactly 0 so instead of doing all our shifts, just return 0.

        // place the resulting sign in the result
        "mov %[result], %[m1]\n"
        "and %[result], #0x80000000\n"

        // get the absolute value of m, adjust back to 23 bits
        "abs %[m1], %[m1]\n"
        "shr %[m1], #6\n"

        // normalize the result
        "encod r10, %[m1]\n"
        "cmp r10, #23 wcz\n"
        "mov r11, r10\n"

        // right shift 
        "if_nc_and_nz sub r11, #23\n"
        "if_nc_and_nz shr %[m1], r11\n"
        "if_nc_and_nz add %[e1], r11\n"
        "if_nc_and_nz jmp #.Lcheck_exp_addf32\n" 

        // left shift, but make sure we have enough room in e to do it. 
        "subr r11, #23\n" // r17 = how much to shift by
        "cmp r11, %[e1] wcz\n" // e will be 0 and the result is subnormal
        "if_nc sub %[e1], #1\n"
        "if_nc shl %[m1], %[e1]\n"
        "if_nc mov %[e1], #0\n"
        "if_nc jmp #.Lcheck_exp_addf32\n"

        "shl %[m1], r11\n"
        "sub %[e1], r11\n"

        ".Lcheck_exp_addf32:\n"
        
        // e >= 0xff, overflow
        "cmps %[e1], #0xff wcz\n"
        "if_nc or %[result], #0x7F800000\n"
        "if_nc jmp #.Lret_addf32\n"

        // e < 0, underflow
        "cmps %[e1], #0 wcz\n"
        "if_c jmp #.Lret_addf32\n"

        "shl %[e1], #23\n"
        "or %[result], %[e1]\n"
        "and %[m1], %[fraction_mask]\n"
        "or %[result], %[m1]\n"

        ".Lret_addf32:\n" // jump location for early returns

        : [result]"+r"(r_r.i), [a]"+r"(a_r.i), [b]"+r"(b_r.i),
          [e1]"+r"(e1),
          [e2]"+r"(e2),
          [m1]"+r"(m1),
          [m2]"+r"(m2),
          [m1_frac]"+r"(m1_frac),
          [m2_frac]"+r"(m2_frac),
          [e1_denorm]"+r"(e1_denorm),
          [e2_denorm]"+r"(e2_denorm),
          [a_NaN]"+r"(a_NaN),
          [a_Inf]"+r"(a_Inf),
          [a_Zero]"+r"(a_Zero),
          [b_NaN]"+r"(b_NaN),
          [b_Inf]"+r"(b_Inf),
          [b_Zero]"+r"(b_Zero),
          [fraction_mask]"+r"(fraction_mask),
          [leading_one]"+r"(leading_one),
          [e]"+r"(e),
          [sign_a]"+r"(sign_a),
          [sign_b]"+r"(sign_b),
          [exp_bits]"+r"(exp_bits),
          [nan_bits]"+r"(nan_bits),
          [e_diff]"+r"(e_diff)
        :: "r10", "r11", "r12"
    );

    return r_r.f;
}

// test data
float_rep f_list[] = {
    {.i = 0x132f8aeb}, // random tiny positive number
    {.i = 0x369caae3}, // random small positive number
    {.i = 0x4a1caae3}, // random large positive number
    {.i = 0x6e1caae3}, // random huge positive number
    {.i = 0x5ff00000}, // random huge positive number
    {.i = 0x5f700000}, // random huge positive number
    {.i = 0xcab5bf7c}, // random negative number
    {.i = 0x002f8aeb}, // denormal positive number
    {.i = 0x802f8aeb}, // denormal negative number
    {.i = 0}, // positive 0
    {.i = 0x80000000}, // negative 0

    {.i = 0x7f800000}, // positive Inf
    {.i = 0xff800000}, // negative Inf
    {.i = 0x7f800001},  // NaN

    {.i = 0x49742400},
    {.i = 0x00400000},
    {.i = 0x3FC00000},
    {.i = 0x3ff00000},
    {.i = 0x3f800000},
    {.i = 0x7f000000},
    {.i = 0x00000001},

    // {.i = 872415232},
    // {.i = 3019898880},
    // {.i = 2139095040},
    // {.i = 4286578688},
    // {.i = 2139095039},
    // {.i = 4286578687},
    // {.i = 8388608},
    // {.i = 2155872256},
    // {.i = 2143289344},
    // {.i = 2147483648},
    // {.i = 0},
    // {.i = 1065353216},
    // {.i = 3212836864},
    // {.i = 71362},
    // {.i = 2147555010},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 2147483648},
    // {.i = 0},
    // {.i = 3738717043},
    // {.i = 1591233395},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 4286578688},
    // {.i = 2139095040},
    // {.i = 2699950222},
    // {.i = 552466574},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 2147483648},
    // {.i = 0},
    // {.i = 3725177487},
    // {.i = 1577693839},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 4286578688},
    // {.i = 2139095040},
    // {.i = 2685894459},
    // {.i = 538410811},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 2147483648},
    // {.i = 0},
    // {.i = 3752452258},
    // {.i = 1604968610},
    // {.i = 3212836864},
    // {.i = 1065353216},
    // {.i = 4286578688},
    // {.i = 2139095040},
    // {.i = 2713380581},
    // {.i = 565896933}
};

#define FRACTION(x) (x & 0x7fffff)
#define SIGN(x) (x >> 31)
#define EXP(x) (((x>>23) & 0xff))

int isnanf(float f) {
    float_rep f_r = {.f = f};
    return ((EXP(f_r.i) == 255) && (FRACTION(f_r.i) != 0));
}
    
int main() {
    printf("$\n"); // start of test character

    volatile float_rep r_old;
    volatile float_rep r_new;

    int num_correct_mul = 0;
    int num_correct_add = 0;
    int num_correct_div = 0;

    printf("t_llvm, t_p2lb\n");
    for (int i = 0; i < sizeof(f_list)/sizeof(float_rep); i++) {
        for (int j = 0; j < sizeof(f_list)/sizeof(float_rep); j++) {
            // multiplication test
            int t0 = CNT;
            r_old.f = f_list[i].f * f_list[j].f;
            t0 = CNT - t0;

            int t1 = CNT;
            r_new.f = __mulsf3_fast(f_list[i].f, f_list[j].f);
            t1 = CNT - t1;

            // printf("%d,%d\n", t0, t1);

            if (r_old.i != r_new.i) {
                int diff = r_old.i - r_new.i;
                if (diff > 1 or diff < -1) {
                    if (isnanf(r_old.f) && isnanf(r_new.f)) {
                        num_correct_mul++;
                    } else {
                        printf("DIFF: ");
                        printf("%x * %x = expected %x, got %x\n", f_list[i].i, f_list[j].i, r_old.i, r_new.i);
                    }
                } else {
                    num_correct_mul++;
                }
            } else {
                num_correct_mul++;
            }

            // addition test
            t0 = CNT;
            r_old.f = f_list[i].f + f_list[j].f;
            t0 = CNT - t0;

            t1 = CNT;
            r_new.f = __addsf3_fast(f_list[i].f, f_list[j].f);
            t1 = CNT - t1;

            // printf("%d,%d\n", t0, t1);

            if (r_old.i != r_new.i) {
                int diff = r_old.i - r_new.i;
                if (diff > 2 or diff < -2) {
                    if (isnanf(r_old.f) && isnanf(r_new.f)) {
                        num_correct_add++;
                    } else {
                        printf("DIFF: ");
                        printf("%x + %x = expected %x, got %x\n", f_list[i].i, f_list[j].i, r_old.i, r_new.i);
                    }
                } else {
                    num_correct_add++;
                }
            } else {
                num_correct_add++;
            }

            // Division test
            t0 = CNT;
            r_old.f = f_list[i].f / f_list[j].f;
            t0 = CNT - t0;

            // time new
            t1 = CNT;
            r_new.f = __divsf3_fast(f_list[i].f, f_list[j].f);
            t1 = CNT - t1;

            // printf("%d,%d\n", t0, t1);

            if (r_old.i != r_new.i) {
                int diff = r_old.i - r_new.i;
                if (diff > 1 or diff < -1) {
                    if (isnanf(r_old.f) && isnanf(r_new.f)) {
                        num_correct_div++;
                    } else {
                        printf("DIFF: ");
                        printf("%x / %x = expected %x, got %x\n", f_list[i].i, f_list[j].i, r_old.i, r_new.i);
                    }
                } else {
                    num_correct_div++;
                }
            } else {
                num_correct_div++;
            }

            waitx(1000);
            
        }
    }

    printf("num_correct_mul = %d\n", num_correct_mul);
    printf("num_correct_add = %d\n", num_correct_add);
    printf("num_correct_div = %d\n", num_correct_div);
    printf("~\n"); // end of test character

    busywait();
}