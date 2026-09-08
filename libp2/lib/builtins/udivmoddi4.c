//===-- udivmoddi4.c - Implement __udivmoddi4 -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __udivmoddi4 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Effects: if rem != 0, *rem = a % b
// Returns: a / b

// Preserve the original unsigned QDIV fast path. For a wide divisor, divide
// normalized operands to estimate the quotient, then correct using the full
// 96-bit product. The estimate is exact or one too large; see step-3 notes.
// Register-pair operands let the compiler manage the ABI and stack argument.
COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int *r) {
    register du_int result __asm__("r30_r31");
    __asm__ volatile(
            "cmp %H[b], #0 wz\n"
    "if_nz  jmp #.Lwide_divisor%=\n"
            "cmp %L[b], #0 wz\n"
    "if_z   jmp #.Lzero_divisor%=\n"
            "cmp %L[b], #1 wz\n"
    "if_z   jmp #.Lunit_divisor%=\n"

            // Split 64/32 into two exact QDIV operations only when needed.
            "mov %H[result], #0\n"
            "cmp %H[a], %L[b] wc\n"
    "if_c   jmp #.Llow_quotient%=\n"
            "qdiv %H[a], %L[b]\ngetqx %H[result]\ngetqy %H[a]\n"
    ".Llow_quotient%=:\n"
            "setq %H[a]\nqdiv %L[a], %L[b]\ngetqx %L[result]\ngetqy %L[a]\n"
            "mov %H[a], #0\njmp #.Lstore_remainder%=\n"

    ".Lwide_divisor%=:\n"
            "cmp %L[a], %L[b] wcz\ncmpx %H[a], %H[b] wcz\n"
    "if_c   mov %L[result], #0\n"
    "if_c   mov %H[result], #0\n"
    "if_c   jmp #.Lstore_remainder%=\n"
            "encod r4, %H[b]\nadd r4, #1\n" // s = 1..32; b >> s fits 32 bits.
            "cmp r4, #32 wz\n"
    "if_z   mov r6, %H[a]\n"
    "if_z   mov r7, #0\n"
    "if_z   mov r5, %H[b]\n"
    "if_z   jmp #.Lestimate%=\n" // P2 shift count 32 would otherwise mean zero.
            "mov r5, #32\nsub r5, r4\n"
            "mov r6, %L[a]\nshr r6, r4\n"
            "mov r7, %H[a]\nshl r7, r5\nor r6, r7\n"
            "mov r7, %H[a]\nshr r7, r4\n"
            "mov %L[result], %L[b]\nshr %L[result], r4\n"
            "mov %H[result], %H[b]\nshl %H[result], r5\nor %L[result], %H[result]\n"
            "mov r5, %L[result]\n"
    ".Lestimate%=:\n"
            "setq r7\nqdiv r6, r5\ngetqx %L[result]\n"

            // Form q*b without losing bit 64 when the estimate is too large.
            "qmul %L[result], %L[b]\ngetqx r6\ngetqy r7\n"
            "qmul %L[result], %H[b]\ngetqx r5\ngetqy r4\n"
            "add r7, r5 wc\naddx r4, #0\n"
            "sub %L[a], r6 wc\nsubx %H[a], r7 wc\n"
            "cmp r4, #0 wz\n" // Preserve subtraction borrow in C.
    "if_nc_and_z jmp #.Lexact_quotient%=\n"
            "sub %L[result], #1\nadd %L[a], %L[b] wc\naddx %H[a], %H[b]\n"
    ".Lexact_quotient%=:\n"
            "mov %H[result], #0\njmp #.Lstore_remainder%=\n"

    ".Lzero_divisor%=:\n"
            // Keep the previous helper extension: max quotient, *r untouched.
            // C division by zero remains undefined.
            "not %L[result], #0\nmov %H[result], %L[result]\njmp #.Ludiv_return%=\n"
    ".Lunit_divisor%=:\n"
            "mov %L[result], %L[a]\nmov %H[result], %H[a]\nmov %L[a], #0\nmov %H[a], #0\n"
    ".Lstore_remainder%=:\n"
            "cmp %[rem], #0 wz\n"
    "if_z   jmp #.Ludiv_return%=\n"
            "setq #1\nwrlong %L[a], %[rem]\n"
    ".Ludiv_return%=:\n"
    : [result] "=&r"(result), [a] "+&r"(a)
    : [b] "r"(b), [rem] "r"(r)
    : "r4", "r5", "r6", "r7", "cc", "memory");
    return result;
}

#if 0
COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int *rem) {
  const unsigned n_uword_bits = sizeof(su_int) * CHAR_BIT;
  const unsigned n_udword_bits = sizeof(du_int) * CHAR_BIT;
  udwords n;
  n.all = a;
  udwords d;
  d.all = b;
  udwords q;
  udwords r;
  unsigned sr;
  // special cases, X is unknown, K != 0
  if (n.s.high == 0) {
    if (d.s.high == 0) {
      // 0 X
      // ---
      // 0 X
      if (rem)
        *rem = n.s.low % d.s.low;
      return n.s.low / d.s.low;
    }
    // 0 X
    // ---
    // K X
    if (rem)
      *rem = n.s.low;
    return 0;
  }
  // n.s.high != 0
  if (d.s.low == 0) {
    if (d.s.high == 0) {
      // K X
      // ---
      // 0 0
      if (rem)
        *rem = n.s.high % d.s.low;
      return n.s.high / d.s.low;
    }
    // d.s.high != 0
    if (n.s.low == 0) {
      // K 0
      // ---
      // K 0
      if (rem) {
        r.s.high = n.s.high % d.s.high;
        r.s.low = 0;
        *rem = r.all;
      }
      return n.s.high / d.s.high;
    }
    // K K
    // ---
    // K 0
    if ((d.s.high & (d.s.high - 1)) == 0) /* if d is a power of 2 */ {
      if (rem) {
        r.s.low = n.s.low;
        r.s.high = n.s.high & (d.s.high - 1);
        *rem = r.all;
      }
      return n.s.high >> __builtin_ctz(d.s.high);
    }
    // K K
    // ---
    // K 0
    sr = clzsi(d.s.high) - clzsi(n.s.high);
    // 0 <= sr <= n_uword_bits - 2 or sr large
    if (sr > n_uword_bits - 2) {
      if (rem)
        *rem = n.all;
      return 0;
    }
    ++sr;
    // 1 <= sr <= n_uword_bits - 1
    // q.all = n.all << (n_udword_bits - sr);
    q.s.low = 0;
    q.s.high = n.s.low << (n_uword_bits - sr);
    // r.all = n.all >> sr;
    r.s.high = n.s.high >> sr;
    r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
  } else /* d.s.low != 0 */ {
    if (d.s.high == 0) {
      // K X
      // ---
      // 0 K
      if ((d.s.low & (d.s.low - 1)) == 0) /* if d is a power of 2 */ {
        if (rem)
          *rem = n.s.low & (d.s.low - 1);
        if (d.s.low == 1)
          return n.all;
        sr = __builtin_ctz(d.s.low);
        q.s.high = n.s.high >> sr;
        q.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
        return q.all;
      }
      // K X
      // ---
      // 0 K
      sr = 1 + n_uword_bits + clzsi(d.s.low) - clzsi(n.s.high);
      // 2 <= sr <= n_udword_bits - 1
      // q.all = n.all << (n_udword_bits - sr);
      // r.all = n.all >> sr;
      if (sr == n_uword_bits) {
        q.s.low = 0;
        q.s.high = n.s.low;
        r.s.high = 0;
        r.s.low = n.s.high;
      } else if (sr < n_uword_bits) /* 2 <= sr <= n_uword_bits - 1 */ {
        q.s.low = 0;
        q.s.high = n.s.low << (n_uword_bits - sr);
        r.s.high = n.s.high >> sr;
        r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
      } else /* n_uword_bits + 1 <= sr <= n_udword_bits - 1 */ {
        q.s.low = n.s.low << (n_udword_bits - sr);
        q.s.high = (n.s.high << (n_udword_bits - sr)) |
                   (n.s.low >> (sr - n_uword_bits));
        r.s.high = 0;
        r.s.low = n.s.high >> (sr - n_uword_bits);
      }
    } else {
      // K X
      // ---
      // K K
      sr = clzsi(d.s.high) - clzsi(n.s.high);
      // 0 <= sr <= n_uword_bits - 1 or sr large
      if (sr > n_uword_bits - 1) {
        if (rem)
          *rem = n.all;
        return 0;
      }
      ++sr;
      // 1 <= sr <= n_uword_bits
      // q.all = n.all << (n_udword_bits - sr);
      q.s.low = 0;
      if (sr == n_uword_bits) {
        q.s.high = n.s.low;
        r.s.high = 0;
        r.s.low = n.s.high;
      } else {
        q.s.high = n.s.low << (n_uword_bits - sr);
        r.s.high = n.s.high >> sr;
        r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
      }
    }
  }
  // Not a special case
  // q and r are initialized with:
  // q.all = n.all << (n_udword_bits - sr);
  // r.all = n.all >> sr;
  // 1 <= sr <= n_udword_bits - 1
  su_int carry = 0;
  for (; sr > 0; --sr) {
    // r:q = ((r:q)  << 1) | carry
    r.s.high = (r.s.high << 1) | (r.s.low >> (n_uword_bits - 1));
    r.s.low = (r.s.low << 1) | (q.s.high >> (n_uword_bits - 1));
    q.s.high = (q.s.high << 1) | (q.s.low >> (n_uword_bits - 1));
    q.s.low = (q.s.low << 1) | carry;
    // carry = 0;
    // if (r.all >= d.all)
    // {
    //      r.all -= d.all;
    //      carry = 1;
    // }
    const di_int s = (di_int)(d.all - r.all - 1) >> (n_udword_bits - 1);
    carry = s & 1;
    r.all -= d.all & s;
  }
  q.all = (q.all << 1) | carry;
  if (rem)
    *rem = r.all;
  return q.all;
}

#endif