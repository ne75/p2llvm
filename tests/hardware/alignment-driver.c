#include "observe.h"
extern unsigned aligned_locals(unsigned);
unsigned inspect_alignment(void *a, void *b, unsigned n) {
    unsigned char *x = a, *y = b;
    unsigned result = (((__UINTPTR_TYPE__)a & 15) == 0);
    result |= (((__UINTPTR_TYPE__)b & 31) == 0) << 1;
    result |= (x[0] == n && x[18] == n+1 && y[0] == n+2 && y[34] == n+3) << 2;
    return result;
}
#ifndef P2_TEST_HOST
extern unsigned alignment_with_offset(unsigned, unsigned);
#endif
void test_body(void) {
    observe("alignment.locals", aligned_locals(23));
    unsigned failures = 0;
    for (unsigned offset = 0; offset < 32; ++offset) {
#ifdef P2_TEST_HOST
        failures += aligned_locals(23) != 7;
#else
        failures += alignment_with_offset(offset, 23) != 7;
#endif
    }
    observe("alignment.all_stack_residues", failures);
}
