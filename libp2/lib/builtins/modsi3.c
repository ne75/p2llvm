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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
COMPILER_RT_ABI si_int __modsi3(si_int a, si_int b) {
    asm(
            "mov r2, r0\n"
            "shr r2, #31 wz\n"    // r2 = 1: result is negative, z = result is positive
            "abs r0, r0\n"
            "abs r1, r1\n"
            "abs r0, r0\n"
            "qdiv r0, r1\n"
            "getqx r31\n"
    "if_nz  neg r31, r31\n"
    : : : "r0", "r1", "r2");  
}
#pragma clang diagnostic pop