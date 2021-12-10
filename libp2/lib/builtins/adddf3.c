//===-- lib/adddf3.c - Double-precision addition ------------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements double-precision soft-float addition.
//
//===----------------------------------------------------------------------===//

#define DOUBLE_PRECISION
#include "fp_add_impl.inc"

COMPILER_RT_ABI double __adddf3(double a, double b) { return __addXf3__(a, b); }
