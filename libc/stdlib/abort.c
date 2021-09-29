#include <stdlib.h>
#include <propeller.h>

void
abort(void)
{
  __unreachable();
  while(1); // include to suppress silly errors that this function shouldn't return
}
