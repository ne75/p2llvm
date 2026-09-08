#ifndef P2_SHIFT_REFERENCE_H
#define P2_SHIFT_REFERENCE_H

struct shift_words { unsigned lo, hi; };

// Independent bit-at-a-time oracle: only 32-bit shifts by 1 or 31.
// No 64-bit arithmetic, variable word shift, or 32-count expression.
static struct shift_words shift_reference(struct shift_words v,
                                           unsigned count, unsigned kind) {
    while (count--) {
        if (kind == 0) {
            v.hi = (v.hi << 1) | (v.lo >> 31);
            v.lo <<= 1;
        } else {
            v.lo = (v.lo >> 1) | (v.hi << 31);
            unsigned sign = kind == 2 ? v.hi & 0x80000000u : 0;
            v.hi = (v.hi >> 1) | sign;
        }
    }
    return v;
}
#endif
