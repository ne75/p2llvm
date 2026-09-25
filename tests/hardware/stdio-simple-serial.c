// Compile the actual serial driver, replacing only pin configuration.
#define _uart_init stdio_test_uart_init
#include "../../libc/drivers/SimpleSerial.c"
