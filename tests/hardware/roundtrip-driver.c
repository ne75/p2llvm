#include "observe.h"
int g;
extern int *address(void);
extern int local(int), branch(int, int);
void use(int *p) { *p += 13; }
void test_body(void) {
    observe("roundtrip.address", address() == &g);
    observe("roundtrip.local", local(7));
    observe("roundtrip.branch_true", branch(1, 7));
    observe("roundtrip.branch_false", branch(0, 7));
}
