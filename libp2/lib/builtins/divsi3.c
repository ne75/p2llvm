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

// Complete assembly leaf: arguments R0/R1, result R31. Keep the optimized
// original register saves and instruction sequence at every optimization level.
COMPILER_RT_ABI __attribute__((naked)) si_int __divsi3(si_int a, si_int b) {
    __asm__ volatile(
            "setq #2\n"
            "wrlong r0, ptra++\n"
            "mov r2, r0\n"
            "xor r2, r1\n"
            "shr r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
            "abs r0, r0\n"
            "abs r1, r1\n"
            "qdiv r0, r1\n"
            "getqx r31\n"
    "if_nz  neg r31, r31\n"
            "setq #2\n"
            "rdlong r0, --ptra\n"
            "reta\n");
}
