//===-- udivdi3.c - Implement __udivdi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __udivdi3 for the compiler_rt library.
//
// Modified for P2LLVM by N. Ermoshkin
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

typedef du_int fixuint_t;
typedef di_int fixint_t;
#include "int_div_impl.inc"

// Returns: a / b

COMPILER_RT_ABI du_int __udivdi3(du_int a, du_int b) {
    du_int *r = 0;
    return __udivmoddi4(a, b, r);
}
