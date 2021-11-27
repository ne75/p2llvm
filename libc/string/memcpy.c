/*
 * @memcpy.c
 * Implementation of string library functions
 *
 * Copyright (c) 2011 Parallax, Inc.
 * Written by Eric R. Smith, Total Spectrum Software Inc.
 * MIT licensed (see terms at end of file)
 */
#include <string.h>
#include <compiler.h>
#include <propeller.h>

#ifdef __p2llvm__

void *memcpy(void *dst, const void *src, size_t n) {
    char *d = (char *)dst;
    const char *s = (const char*)src;

    // might be a smarter way to do this using the FIFO and such
    if ((d != 0) && (s != 0)) {
        while(n) {
            //Copy byte by byte
            *(d++)= *(s++);
            --n;
        }
    }

    return dst;
}

#else
#define ALIGNED(a) ( 0 == ( ((unsigned)(a)) & (sizeof(long)-1) ) )
#define HUBMEM(a)  ( 0 == ( ((unsigned)(a)) & 0xFFF00000 ) )

#define MINBLOCKSIZE (2*(sizeof(long)))

#if defined(__PROPELLER_CMM__)
__attribute__((fcache))
#endif
void *
memcpy(void *dest_p, const void *src_p, size_t n)
{
  void *orig_dest = dest_p;

#if defined(__PROPELLER_USE_XMM__)
  extern void *_copy_from_xmm(void *dest, const void *src, size_t n);
  if (HUBMEM(dest_p) && !HUBMEM(src_p))
    return _copy_from_xmm(dest_p, src_p, n);
#endif

  /* do a quick copy if src and dest are aligned */
  if ( ALIGNED(dest_p) && ALIGNED(src_p) && n > MINBLOCKSIZE) {
    long *dst = dest_p;
    const long *src = src_p;
    do
      {
	*dst++ = *src++;
	*dst++ = *src++;
	n -= MINBLOCKSIZE;
      }
    while (n > MINBLOCKSIZE);
    dest_p = dst;
    src_p = src;
  }

  {
    char *dst = dest_p;
    const char *src = src_p;
    
    while (n > 0) {
      *dst++ = *src++;
      --n;
    }
  }

  return orig_dest;
}
#endif

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
