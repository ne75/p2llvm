//===-- muldi3.c - Implement __muldi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __muldi3 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a * b. The compiler owns register saves and the return sequence.
// Early-clobber pairs keep the modified operands and result disjoint.

COMPILER_RT_ABI di_int __muldi3(di_int a, di_int b) {
    register di_int result __asm__("r30_r31");
    __asm__ volatile(
            "mov r4, %H[a]\n"
            "shr r4, #31\n"    // sign of a
            "mov r5, %H[b]\n"
            "shr r5, #31\n"    // sign of b
            "mov r6, r4\n"
            "xor r6, r5\n"    // sign of result
            
            "cmp r4, #0    wz\n"
    "if_z   jmp #.Lskip_neg_a%=\n"    // a is positive?
            // a = -a;
            "not %L[a], %L[a]\n"
            "not %H[a], %H[a]\n"
            "add %L[a], #1    wc\n"
            "addx %H[a], #0\n"

    ".Lskip_neg_a%=:"

            "cmp r5, #0    wz\n"
    "if_z   jmp #.Lskip_neg_b%=\n"    // b is positive?

            // b = -b;
            "not %L[b], %L[b]\n"
            "not %H[b], %H[b]\n"
            "add %L[b], #1    wc\n"
            "addx %H[b], #0\n"
    ".Lskip_neg_b%=:"

            // a0*b0
            "qmul %L[a], %L[b]\n"
            "getqx %L[result]\n"
            "getqy %H[result]\n"

            // a1*b0
            "qmul %H[a], %L[b]\n"
            "getqx r4\n" // r4 = upper 32 of a1*b0 << 32
            "add %H[result], r4\n"
            
            // a0*b1
            "qmul %L[a], %H[b]\n"
            "getqx r4\n" // r4 = upper 32 of a0*b1 << 32
            "add %H[result], r4\n"

            "cmp r6, #0    wz\n"
    "if_z   jmp #.Lskip_neg_r%=\n"    // is result positive?

            "not %L[result], %L[result]\n"
            "not %H[result], %H[result]\n"
            "add %L[result], #1   wc\n"
            "addx %H[result], #0\n"
    ".Lskip_neg_r%=:"
    
    : [result] "=&r"(result), [a] "+&r"(a), [b] "+&r"(b)
    : : "r4", "r5", "r6", "cc");
    return result;
}
