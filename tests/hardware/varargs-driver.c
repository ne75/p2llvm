#include "observe.h"
extern int call_sum(int, int), call_aggregate(int, int);
extern int call_mixed(int, int), call_stack_narrow(void);
void test_body(void) {
    observe("varargs.ints", call_sum(-4, 19));
    observe("varargs.aggregate", call_aggregate(0x1234, 0x5678));
    observe("varargs.packed_then_int", call_mixed(0x1234, 0x5678));
    observe("stack.narrow", call_stack_narrow());
}
