#include "stdio-lock-model.h"
// Match the two locks reserved by P2 startup, without assuming stream IDs.
static unsigned allocated = 3, held;
unsigned model_errors;
volatile int __sys_lock = 1;
unsigned _locknew(void) {
    for (unsigned id = 0; id < 16; ++id)
        if (!(allocated & (1u << id))) {
            allocated |= 1u << id;
            return id;
        }
    return ~0u;
}
void _lockret(unsigned id) {
    if (id >= 16 || !(allocated & (1u << id))) { ++model_errors; return; }
    allocated &= ~(1u << id);
}
int _locktry(unsigned id) {
    if (id >= 16 || !(allocated & (1u << id))) { ++model_errors; return 0; }
    unsigned bit = 1u << id;
    held |= bit;
    return 1;
}
void _lock(unsigned id) {
    if (id < 16 && !_locktry(id)) ++model_errors;
}
void _unlock(unsigned id) {
    if (id < 16) held &= ~(1u << id);
}
unsigned stdio_test_is_held(unsigned id) {
    return (held >> id) & 1;
}
