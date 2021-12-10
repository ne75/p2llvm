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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b) { 
    // r0 = a, r1 = b
    asm(
            "mov $r2, $r0\n"
            "xor $r2, $r1\n"
            "shr $r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
            "abs $r0, $r0\n"
            "abs $r1, $r1\n"
            "qdiv $r0, $r1\n"
            "getqx $r31\n"
    "if_nz  neg $r31, $r31\n"
    : : : "r0", "r1", "r2");      
}
#pragma clang diagnostic pop