#include "observe.h"
extern unsigned branch_span(unsigned);
void test_body(void) {
    observe("branch.taken", branch_span(0));
    observe("branch.fallthrough", branch_span(1));
}
