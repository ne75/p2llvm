//===-- negdi2.c - Implement __negdi2 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __negdi2 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: -a

COMPILER_RT_ABI di_int __negdi2(di_int a) {
    // Note: this routine is here for API compatibility; any sane compiler
    // should expand it inline.
    register di_int result __asm__("r30_r31");
    __asm__ volatile(
        "not %L0, %L1\n"
        "not %H0, %H1\n"
        "add %L0, #1 wc\n"
        "addx %H0, #0\n"
        : "=&r"(result) : "r"(a) : "cc");
    return result;
}
