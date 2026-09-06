#include "observe.h"
extern unsigned constant_load(void);
extern void constant_store(void);
void test_body(void) {
    unsigned saved, value;
    __asm__ volatile("rdlong %0, ##256\nwrlong ##0x12345678, ##256"
                     : "=r"(saved) : : "memory");
    value = constant_load();
    __asm__ volatile("wrlong %0, ##256" : : "r"(saved) : "memory");
    observe("absolute.load", value);
    __asm__ volatile("rdlong %0, ##260" : "=r"(saved) : : "memory");
    constant_store();
    __asm__ volatile("rdlong %0, ##260\nwrlong %1, ##260"
                     : "=&r"(value) : "r"(saved) : "memory");
    observe("absolute.store", value);
}
