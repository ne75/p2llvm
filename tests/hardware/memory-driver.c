#include "observe.h"
extern unsigned memory_copy(unsigned), memory_move(unsigned, unsigned), memory_set_zero(void);
extern unsigned memory_return;
void test_body(void) {
    observe("copy.zero", memory_copy(0));
    observe("copy.one", memory_copy(1));
    observe("copy.three", memory_copy(3));
    observe("copy.four", memory_copy(4));
    observe("copy.511", memory_copy(511));
    observe("copy.512", memory_copy(512));
    observe("copy.513", memory_copy(513));
    observe("copy.return", memory_return);
    observe("move.left", memory_move(0, 9));
    observe("move.right", memory_move(1, 9));
    observe("move.return", memory_return);
    observe("move.zero", memory_move(1, 0));
    observe("set.zero", memory_set_zero());
    observe("set.return", memory_return);
}
