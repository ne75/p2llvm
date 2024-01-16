/*
 * @memcpy.c
 * Implementation of string library functions
 *
 * Copyright (c) 2011 Parallax, Inc.
 * Written by Eric R. Smith, Total Spectrum Software Inc.
 * MIT licensed (see terms at end of file)
 */
#include <propeller.h>
#include <stdio.h>

__attribute__ ((section ("lut"), cogtext, no_builtin("memcpy")))
void *memcpy(void *dst, const void *src, size_t n) {
    if ((dst == NULL) || (src == NULL))
        return dst;

    void *d = dst;

#define FAST_MEMCPY
#ifdef FAST_MEMCPY
    int n_longs = n>>2;
    int n_blocks = n_longs >> 7;
    int extra_longs = n_longs & 127;
    int extra_bytes = n & 3;
    int setq_size = 127;

    // our blocks are 128 longs in size (1<<7)
    // 1. copying all full blocks using setq 128
    // 2. copy the remaining longs using setq extra_longs
    // 3. copy remaining bytes using looped rdbytes
    //
    // for autoincrementing pointers, we need to use ptra or ptrb. so, save ptra (our stack pointer)
    // to pb, then restore it at the end of this block.
    asm volatile (
        "mov pb, ptra\n"
        "mov ptra, %[src]\n"
        "mov ptrb, %[dst]\n"
        "tjz %[n_blocks], #.Lcopy_extra_longs\n"

        // copy blocks
        ".Lcopy_blocks:\n"
        "setq %[setq_size]\n"
        "rdlong $0, ptra++\n"
        "setq %[setq_size]\n"
        "wrlong $0, ptrb++\n"
        "djnz %[n_blocks], #.Lcopy_blocks\n" // we have more blocks, keep going

        // set up to copy another block of smaller size
        ".Lcopy_extra_longs:\n"
        "tjz %[extra_longs], #.Lcopy_extra_bytes\n"

        "mov %[setq_size], %[extra_longs]\n"
        "sub %[setq_size], #1\n" // -1 because setq is how many extra to copy 
        "mov %[extra_longs], #0\n"
        "add %[n_blocks], #1\n" // copy 1 more block with a different size. 
        "jmp #.Lcopy_blocks\n"

        ".Lcopy_extra_bytes:\n"
        "tjz %[extra_bytes], #.Lend\n"
        "rep #2, %[extra_bytes]\n"
        "rdbyte $0, ptra++\n"
        "wrbyte $0, ptrb++\n"

        ".Lend:\n"
         "mov ptra, pb\n"

        :   [dst]"+r"(dst),
            [src]"+r"(src), 
            [n_longs]"+r"(n_longs), 
            [n_blocks]"+r"(n_blocks), 
            [extra_longs]"+r"(extra_longs), 
            [extra_bytes]"+r"(extra_bytes),
            [setq_size]"+r"(setq_size)
        ::
    );

#else
    unsigned int b;
    asm volatile ("rdfast #0, %[src]\n"
             ".L1: rfbyte %[b]\n"
                  "wrbyte %[b], %[dst]\n"
                  "add %[dst], #1\n"
                  "djnz %[n], #.L1\n"
                  :[src]"+r"(src), [dst]"+r"(dst), [b]"+r"(b), [n]"+r"(n):);
#endif

    return d;
}


/* +--------------------------------------------------------------------
 * ¦  TERMS OF USE: MIT License
 * +--------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * +--------------------------------------------------------------------
 */
