//===-- fixunssfdi.c - Implement __fixunssfdi -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#define SINGLE_PRECISION
#include "fp_lib.h"

typedef du_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI du_int __fixunssfdi(fp_t a) { return __fixuint(a); }
