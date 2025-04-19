#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/size_t.h>
#include <sys/wchar_t.h>
#include <sys/null.h>
#include <compiler.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

  double atof(const char *);
  int    atoi(const char *);
  long   atol(const char *);
  long long atoll(const char *);

  long double strtold(const char *nptr, char **endptr);
  double strtod(const char *nptr, char **endptr);
  float  strtof(const char *nptr, char **endptr);

  long strtol(const char *nptr, char **endptr, int base);
  unsigned long strtoul(const char *nptr, char **endptr, int base);
  long long strtoll(const char *nptr, char **endptr, int base);
  unsigned long long strtoull(const char *nptr, char **endptr, int base);

// TODO: update for new random number generator
#define RAND_MAX    0x7fff
  int rand(void);
  void srand(unsigned int seed);

  void *malloc(size_t n);
  void *calloc(size_t, size_t);
  void *realloc(void *, size_t);
  void free(void *);

// No need to hub vs non-hub calls
//   void *_hubmalloc(size_t n);
//   void *_hubcalloc(size_t, size_t);
//   void *_hubrealloc(void *, size_t);
//   void _hubfree(void *);

// #if !defined(__STRICT_ANSI__)
//   void *hubmalloc(size_t n);
//   void *hubcalloc(size_t, size_t);
//   void *hubrealloc(void *, size_t);
//   void hubfree(void *);
// #endif

  int atexit(void (*func)(void));
  _NORETURN void exit(int status);
  _NORETURN void abort(void);
  _NORETURN void _Exit(int status);

  _CONST int abs(int i);
  _CONST long labs(long l);
  _CONST long long llabs(long long ll);

  typedef struct {
    int quot, rem;
  } div_t;

  typedef struct {
    long int quot, rem;
  } ldiv_t;

  typedef struct {
    long long quot, rem;
  } lldiv_t;

  div_t div(int num, int denom);
  ldiv_t ldiv(long num, long denom);
  lldiv_t lldiv(long long num, long long denom);

  void qsort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
  void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *));

  char *getenv(const char *name);

  /* multibyte character functions */
  extern int _mb_cur_max;
#define MB_CUR_MAX _mb_cur_max
#define MB_LEN_MAX 4  /* in Unicode up to 4 UTF-8 bytes per unicode wchar_t */

  int mblen(const char *s, size_t n);
  int mbtowc(wchar_t * __restrict pwc, const char * __restrict s, size_t n);
  size_t mbstowcs(wchar_t *dest, const char *src, size_t n);

  /* not implemented */
  int system(const char *command);

#if defined(_POSIX_SOURCE) || defined(_GNU_SOURCE)
  /* some non-ANSI functions */
  int putenv(char *string);
#endif

#if defined(__cplusplus)
}
#endif

#endif
