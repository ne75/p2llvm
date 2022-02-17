//===-- lib/floatsidf.c - integer -> double-precision conversion --*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements integer to double-precision conversion for the
// compiler-rt library in the IEEE-754 default round-to-nearest, ties-to-even
// mode.
//
//===----------------------------------------------------------------------===//

#define DOUBLE_PRECISION
#include "fp_lib.h"

#include "int_lib.h"

fp_t ___floatsidf(si_int a) {

  const int aWidth = sizeof a * CHAR_BIT;

  // Handle zero as a special case to protect clz
  if (a == 0)
    return fromRep(0);

  // All other cases begin by extracting the sign and absolute value of a
  rep_t sign = 0;
  if (a < 0) {
    sign = signBit;
    a = -a;
  }

  // Exponent of (fp_t)a is the width of abs(a).
  const int exponent = (aWidth - 1) - clzsi(a);
  rep_t result;

  // Shift a into the significand field and clear the implicit bit.  Extra
  // cast to unsigned int is necessary to get the correct behavior for
  // the input INT_MIN.
  const int shift = significandBits - exponent;
  result = (rep_t)(su_int)a << shift ^ implicitBit;

  // Insert the exponent
  result += (rep_t)(exponent + exponentBias) << significandBits;
  // Insert the sign bit and return
  return fromRep(result | sign);
}

COMPILER_RT_ABI fp_t __floatsidf(si_int a) {
  return ___floatsidf(a);
}