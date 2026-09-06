//===-- modsi3.c - Implement __modsi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __modsi3 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a % b

COMPILER_RT_ABI si_int __modsi3(si_int a, si_int b) {
    // Declare the ABI return register and modified inputs to the compiler.
    register si_int result __asm__("r31");
    __asm__ volatile(
            "mov r2, %1\n"
            "shr r2, #31 wz\n"  // The remainder has the dividend's sign.
            "abs %1, %1\n"      // P2 ABS also gives INT_MIN's unsigned magnitude.
            "abs %2, %2\n"
            "qdiv %1, %2\n"
            "getqx %0\n"        // Consume the quotient before reading the remainder.
            "getqy %0\n"
    "if_nz  neg %0, %0\n"
    : "=r"(result), "+r"(a), "+r"(b)
    :
    : "r2", "cc");
    return result;
}
