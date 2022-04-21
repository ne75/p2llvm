/*
 * @memmove.c
 * Implementation of string library functions
 *
 * Copyright (c) 2011 Parallax, Inc.
 * Written by Eric R. Smith, Total Spectrum Software Inc.
 * MIT licensed (see terms at end of file)
 */
#include <propeller.h>
#include <stdio.h>

/*
 * this function has to handle overlapping regions correctly
 */

__attribute__ ((section ("lut"), cogtext, no_builtin("memmove")))
void *memmove(void *dest_p, const void *src_p, size_t n)
{
  void *orig_dest = dest_p;
  unsigned int b;

  const char *src = src_p;
  char *dst = dest_p;

  if ( (unsigned long)dst < (unsigned long)src )
  {
    asm volatile ("rdfast #0, %[src]\n"
             ".L1: rfbyte %[b]\n"
                  "wrbyte %[b], %[dst]\n"
                  "add %[dst], #1\n"
                  "djnz %[n], #.L1\n"
                  :[src]"+r"(src), [dst]"+r"(dst), [b]"+r"(b), [n]"+r"(n):);
  }
  else
  {
      /* copy backwards */
      dst += n-1;
      src += n-1;
      asm volatile (
          ".L2: rdbyte %[b], %[src]\n"
          "wrbyte %[b], %[dst]\n"
          "sub %[src], #1\n"
          "sub %[dst], #1\n"
          "djnz %[n], #.L2\n"
          :[src]"+r"(src), [dst]"+r"(dst), [b]"+r"(b), [n]"+r"(n):);

  }

  return orig_dest;
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
