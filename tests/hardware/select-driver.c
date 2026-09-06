#include "observe.h"
#include <limits.h>

extern int select_slt32(int, int, int, int), select_sle32(int, int, int, int);
extern int select_sgt32(int, int, int, int), select_sge32(int, int, int, int);
extern int select_ult32(unsigned, unsigned, int, int);
extern int select_slt32_imm(int, int, int), select_ult32_imm(unsigned, int, int);
extern int select_slt64(long long, long long, int, int);
extern int select_ult64(unsigned long long, unsigned long long, int, int);
extern int select_slt64_imm(long long, int, int);

void test_body(void) {
    observe("slt.true", select_slt32(-4, 2, 19, 23));
    observe("slt.false", select_slt32(4, 2, 19, 23));
    observe("slt.negative_true", select_slt32(-5, -4, 19, 23));
    observe("slt.negative_false", select_slt32(-4, -5, 19, 23));
    observe("slt.min_max", select_slt32(INT_MIN, INT_MAX, 19, 23));
    observe("slt.max_min", select_slt32(INT_MAX, INT_MIN, 19, 23));
    observe("slt.equal", select_slt32(2, 2, 19, 23));
    observe("sle.equal", select_sle32(2, 2, 19, 23));
    observe("sle.true", select_sle32(-4, 2, 19, 23));
    observe("sle.false", select_sle32(2, -4, 19, 23));
    observe("sgt.false", select_sgt32(-4, 2, 19, 23));
    observe("sgt.true", select_sgt32(2, -4, 19, 23));
    observe("sgt.equal", select_sgt32(2, 2, 19, 23));
    observe("sge.false", select_sge32(-4, 2, 19, 23));
    observe("sge.true", select_sge32(2, -4, 19, 23));
    observe("sge.equal", select_sge32(2, 2, 19, 23));
    observe("ult.false", select_ult32(0xfffffffcu, 2, 19, 23));
    observe("ult.true", select_ult32(2, 0xfffffffcu, 19, 23));
    observe("ult.equal", select_ult32(2, 2, 19, 23));
    observe("slt.imm_true", select_slt32_imm(-4, 19, 23));
    observe("slt.imm_equal", select_slt32_imm(2, 19, 23));
    observe("ult.imm_false", select_ult32_imm(0xfffffffcu, 19, 23));
    observe("ult.imm_true", select_ult32_imm(1, 19, 23));
    observe("ult.imm_equal", select_ult32_imm(2, 19, 23));
    observe("slt64.low_sign", select_slt64(0x80000000LL, 1, 19, 23));
    observe("slt64.borrow", select_slt64(0x100000000LL, 0xffffffffLL, 19, 23));
    observe("slt64.negative", select_slt64(-1, 1, 19, 23));
    observe("slt64.min_max", select_slt64(LLONG_MIN, LLONG_MAX, 19, 23));
    observe("ult64.high_sign", select_ult64(0x8000000000000000ULL, 1, 19, 23));
    observe("ult64.true", select_ult64(1, 0x8000000000000000ULL, 19, 23));
    observe("slt64.imm_low_sign", select_slt64_imm(0x180000000LL, 19, 23));
    observe("slt64.imm_below", select_slt64_imm(0x100000001LL, 19, 23));
    observe("slt64.imm_equal", select_slt64_imm(0x100000002LL, 19, 23));
    observe("slt64.imm_negative", select_slt64_imm(-1, 19, 23));
}
