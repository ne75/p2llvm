//===-- lib/subsf3.c - Single-precision subtraction ---------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements single-precision soft-float subtraction.
//
//===----------------------------------------------------------------------===//

#define SINGLE_PRECISION
#include "fp_lib.h"

// Subtraction; flip the sign bit of b and add.
COMPILER_RT_ABI fp_t __subsf3(fp_t a, fp_t b) {
  return __addsf3(a, fromRep(toRep(b) ^ signBit));
}