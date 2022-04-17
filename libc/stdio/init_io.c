/*
 * default _InitIO function to set up stdio, stderr, etc.
 */

#include <driver.h>
#include <compiler.h>
#include <sys/thread.h>

/* this does not need to be in the _TLS area, because
 * all threads share stdin
 */
static unsigned char linebuf[80];

/* initialize I/O */
void _InitIO(void)
{
  int _flock;

  _flock = _locknew();
  /* open stdin */
  __fopen_driver(stdin, _driverlist[0], "", "r");
  /* make it "cooked" input, and give it a decent sized buffer */
  stdin->_flag |= _IOCOOKED;
  stdin->_base = stdin->_ptr = linebuf;
  stdin->_bsiz = sizeof(linebuf);

  /* open stdout */
  __fopen_driver(stdout, _driverlist[0], "", "w");
  /* open stderr */
  __fopen_driver(stderr, _driverlist[0], "", "w");

  // make sure there clear to use
  for (int i=3;i<FOPEN_MAX;i++)
  {
    __files[i]._base = NULL;
    __files[i]._ptr = NULL;
    __files[i]._flag = 0;
    __files[i]._drv = 0;
    __files[i]._lock = _flock;
  }
}
