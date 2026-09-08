// ====-- ashldi3.c - Implement __ashldi3 ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __ashldi3 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a << b

// Precondition:  0 <= b < bits_in_dword

COMPILER_RT_ABI di_int __ashldi3(di_int a, int b) {
    register di_int result __asm__("r30_r31");
    __asm__ volatile(
            "cmp %[count], #32 wc\n"
    "if_nc  jmp #.Lge32%=\n"
            // hi = hi_in << n | (low_in >> (32-n))
            // lo = lo_in << n

            "mov %L[result], %L[a]\n"
            "shl %L[result], %[count]\n"

            "mov %H[result], %H[a]\n"
            "shl %H[result], %[count]\n"
            // For 1..31, -n has the same low five bits as 32-n.
            // Z detects n=0; reuse count after the same-word shifts finish.
            "neg %[count], %[count] wz\n"
    "if_nz  shr %L[a], %[count]\n"
    "if_nz  or %H[result], %L[a]\n"

            "jmp #.Lret%=\n"

    ".Lge32%=:\n"
            // n >= 32
            "sub %[count], #32\n"

            // hi = lo_in << (n-32)
            // lo = 0
            "mov %L[result], #0\n"
            "mov %H[result], %L[a]\n"
            "shl %H[result], %[count]\n"

    ".Lret%=:\n"
    : [result] "=&r"(result), [a] "+&r"(a), [count] "+&r"(b)
    : : "cc");
    return result;
}
