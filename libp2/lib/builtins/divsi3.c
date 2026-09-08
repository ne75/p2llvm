//===-- divsi3.c - Implement __divsi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __divsi3 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a / b

#define fixint_t si_int
#define fixuint_t su_int
// On CPUs without unsigned hardware division support,
//  this calls __udivsi3 (notice the cast to su_int).
// On CPUs with unsigned hardware division support,
//  this uses the unsigned division instruction.
#define COMPUTE_UDIV(a, b) ((su_int)(a) / (su_int)(b))
#include "int_div_impl.inc"

// Declare the return register and all modified operands; let the compiler
// generate register saves, stack adjustments and the return instruction.
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b) {
    register si_int result __asm__("r31");
    __asm__ volatile(
            "mov r2, %1\n"
            "xor r2, %2\n"
            "shr r2, #31 wz\n"
            "abs %1, %1\n"
            "abs %2, %2\n"
            "qdiv %1, %2\n"
            "getqx %0\n"
    "if_nz  neg %0, %0\n"
    : "=r"(result), "+&r"(a), "+&r"(b)
    : : "r2", "cc");
    return result;
}
