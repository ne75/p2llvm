#include "observe.h"
extern int load_boolean(unsigned char *);
extern int signed_compare(long long, long long), signed_overflow(int, int);
extern int count_leading_zeros(unsigned), indirect_branch(int);
void test_body(void) {
    unsigned char b = 0;
    observe("i1.zero", load_boolean(&b));
    b = 1; observe("i1.one", load_boolean(&b));
    observe("signed64.borrow", signed_compare(0x80000000LL, 0));
    observe("signed64.sign", signed_compare(-0x100000001LL, -1));
    observe("signed_product.ok", signed_overflow(-1, 1));
    observe("signed_product.overflow", signed_overflow(-2147483647-1, -1));
    observe("clz.zero", count_leading_zeros(0));
    observe("clz.one", count_leading_zeros(1));
    observe("indirect.zero", indirect_branch(0));
    observe("indirect.other", indirect_branch(7));
}
