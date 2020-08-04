/*
 * very simple printf, adapted from one written by me
 * for the MiNT OS long ago
 * placed in the public domain
 *   - Eric Smith
 * Propeller specific adaptations
 * Copyright (c) 2011 Parallax, Inc.
 * Written by Eric R. Smith, Total Spectrum Software Inc.
 * MIT licensed (see terms at end of file)
 */

#include <stdarg.h>
#include <compiler.h>

#include "propeller2.h"

#define putchar uart_putc

int isdigit(int c) {
  if (c >= '0' && c <= '9')
    return c;
  else
    return 0;
}

/*
 * very simple printf -- just understands a few format features
 * does c,s,u,d,x
 */

#define ULONG unsigned long
#define LONG long

static int
PUTC(int c, int width) {
  int put = 0;

  putchar(c); put++;
  while (--width > 0) {
    putchar(' ');
    put++;
  }
  return put;
}

static int
PUTS(const char *s, int width) {
  int put = 0;

  while (*s) {
    putchar(*s++); put++;
    width--;
  }
  while (width-- > 0) {
    putchar(' '); put++;
  }
  return put;
}

static int PUTL(ULONG u, int base, int width, int fill_char) __attribute__((noinline));

static int
PUTL(ULONG u, int base, int width, int fill_char)
{
  int put = 0;
  char obuf[24]; /* 64 bits -> 22 digits maximum in octal */
  char *t;

  t = obuf;

  const char *chars = "0123456789abcdef";

  do {
    *t++ = chars[(u % base)];
    u /= base;
    width--;
  } while (u > 0);

  while (width-- > 0) {
    putchar(fill_char); put++;
  }

  while (t != obuf) {
    putchar(*--t);
    put++;
  }
  return put;
}

static int
_doprnt( const char *fmt, va_list args )
{
   char c, fill_char;
   char *s_arg;
   unsigned int i_arg;
   ULONG l_arg;
   int width, long_flag;
   int outbytes = 0;
   int base;

   while( (c = *fmt++) != 0 ) {
     if (c != '%') {
       outbytes += PUTC(c, 1);
       continue;
     }
     c = *fmt++;
     width = 0;
     long_flag = 0;
     fill_char = ' ';
     if (c == '0') fill_char = '0';
     while (c && isdigit(c)) {
       width = 10*width + (c-'0');
       c = *fmt++;
     }
     /* for us "long int" and "int" are the same size, so
  we can ignore one 'l' flag; use long long if two
    'l flags are seen */
     while (c == 'l' || c == 'L') {
       long_flag++;
       c = *fmt++;
     }
     if (!c) break;

     switch (c) {
     case '%':
       outbytes += PUTC(c, width);
       break;
     case 'c':
       i_arg = va_arg(args, unsigned int);
       outbytes += PUTC(i_arg, width);
       break;
     case 's':
       s_arg = va_arg(args, char *);
       outbytes += PUTS(s_arg, width);
       break;
     case 'd':
     case 'x':
     case 'u':
       base = (c == 'x') ? 16 : 10;
       l_arg = va_arg(args, unsigned int);
       if (c == 'd') {
        if (((long)l_arg) < 0) {
           outbytes += PUTC('-', 1);
           width--;
           l_arg = (unsigned long)(-((long)l_arg));
         }
       }
       outbytes += PUTL(l_arg, base, width, fill_char);
       break;
     }
   }
   return outbytes;
}

int light_printf(const char *fmt, ...)
{
    va_list args;
    int r;
    va_start(args, fmt);
    r = _doprnt(fmt, args);
    va_end(args);
    return r;
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
