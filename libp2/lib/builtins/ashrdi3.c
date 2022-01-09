//===-- ashrdi3.c - Implement __ashrdi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __ashrdi3 for the compiler_rt library.
// 
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: arithmetic a >> b

// Precondition:  0 <= b < bits_in_dword

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
COMPILER_RT_ABI di_int __ashrdi3(di_int a, int b) {
    asm(
            "cmp r2, #32   wc\n"
    "if_nc  jmp #.Lge32\n"
            // if n < 32
            "mov r3, #32\n"
            "sub r3, r2\n"
            
            // hi = hi_in >> n
            // lo = lo_in >> n | (hi_in << (32-n))

            "mov r31, r1\n"
            "sar r31, r2\n"

            "mov r30, r0\n"
            "sar r30, r2\n"
            "shl r1, r3\n"
            "or r30, r1\n"

            "jmp #.Lret\n"

    ".Lge32:\n"
            // n >= 32
            "mov r3, r2\n"
            "sub r3, #32\n"

            // hi = 32 hi_in[31]
            // lo = hi_in >> (n-32)
            "mov r31, r1\n"
            "sar r31, #31\n"
            "mov r30, r1\n"
            "sar r30, r3\n"

    ".Lret:\n"
    : : : "r0", "r1", "r2", "r3");
}
#pragma clang diagnostic pop
