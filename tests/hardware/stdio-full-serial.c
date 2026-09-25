// Exercise real driver bookkeeping without starting or stopping physical cogs.
#define _uart_init stdio_test_uart_init
#define _coginit stdio_test_coginit
#define malloc stdio_test_malloc
#define free stdio_test_free
#include <propeller.h>
#undef cogstop
#define cogstop(id) stdio_test_cogstop(id)
void stdio_test_cogstop(unsigned id);
#include "../../libc/drivers/FdSerial.c"
