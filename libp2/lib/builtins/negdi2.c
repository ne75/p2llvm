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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
COMPILER_RT_ABI di_int __negdi2(di_int a) {
    // Note: this routine is here for API compatibility; any sane compiler
    // should expand it inline.
    asm(
        "not $r30, $r0\n"
        "not $r31, $r1\n"
        "add $r30, #1   wc\n"
        "addx $r31, #0\n"
    );
}
#pragma clang diagnostic pop