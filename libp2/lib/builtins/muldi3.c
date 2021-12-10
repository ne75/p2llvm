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

// Returns: a * b

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
COMPILER_RT_ABI di_int __muldi3(di_int a, di_int b) {
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
            "add $r2, #1    wc\n"
            "addx $r3, #0\n"
    ".Lskip_neg_b:"

            // a0*b0
            "qmul $r0, $r2\n"
            "getqx $r30\n"
            "getqy $r31\n"

            // a1*b0
            "qmul $r1, $r2\n"
            "getqx $r4\n" // r4 = upper 32 of a1*b0 << 32
            "add $r31, $r4\n"
            
            // a0*b1
            "qmul $r0, $r3\n"
            "getqx $r4\n" // r4 = upper 32 of a0*b1 << 32
            "add $r31, $r4\n"

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