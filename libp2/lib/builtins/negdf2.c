//===-- lib/negdf2.c - double-precision negation ------------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements double-precision soft-float negation.
//
//===----------------------------------------------------------------------===//

#define DOUBLE_PRECISION
#include "fp_lib.h"

COMPILER_RT_ABI fp_t __negdf2(fp_t a) { return fromRep(toRep(a) ^ signBit); }