/*
 * @memset.c
 * Implementation of string library functions
 *
 * Copyright (c) 2011 Parallax, Inc.
 * Written by Eric R. Smith, Total Spectrum Software Inc.
 * MIT licensed (see terms at end of file)
 */
#include <stddef.h>

__attribute__ ((section ("lut"), cogtext, no_builtin("memset")))
void *memset(void *dst, int c, size_t n) {
    if (!n)
        return dst;
    // Run in LUT: HUB execution owns the FIFO. Keep the original byte loop;
    // REP would defer interrupts for the whole fill. RDFAST with D[31]=0
    // completes pending writes before the caller can read or execute in HUB.
    asm volatile("wrfast #0, %[dst]\n"
                 ".Lmemset_byte%=: wfbyte %[value]\n"
                 "djnz %[count], #.Lmemset_byte%=\n"
                 "rdfast #0, %[dst]"
                 : [count] "+&r"(n) : [dst] "r"(dst), [value] "r"(c)
                 : "memory");
    return dst;
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
