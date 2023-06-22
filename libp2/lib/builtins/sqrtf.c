#define SINGLE_PRECISION
#include "fp_lib.h"

static const fp_t one = 1.0, tiny=1.0e-30;

fp_t __attribute__ ((noinline)) __sqrtf(fp_t x) {
    rep_t ix = toRep(x);
    int exp = (ix & exponentMask) >> significandBits;
    rep_t sig = ix & significandMask;

    fp_t z;
	int32_t s,q,m,t,i;
	uint32_t r;

    // handle Inf/NaN
    if (exp == maxExponent) {
        if (ix & signBit) return 0.0f/0.0f; // -Inf or NaN, return NaN
        if (sig == 0) return x; // +Inf return unmodified
        return 0.0f/0.0f;
    }

    // handle +/-0
    if (exp == 0 && sig == 0) return x;

    // handle < 0
    if (ix & signBit) return 0.0/0.0f;

    // handle the rest
    /* normalize x */
	m = (ix>>23);
	if (m==0) {				/* subnormal x */
	    for(i=0;(ix&0x00800000)==0;i++) ix<<=1;
	    m -= i-1;
	}
	m -= 127;	/* unbias exponent */
	ix = (ix&0x007fffff)|0x00800000;
	if(m&1)	/* odd m, double x to make it even */
	    ix += ix;
	m >>= 1;	/* m = [m/2] */

    /* generate sqrt(x) bit by bit */
	ix += ix;
	q = s = 0;		/* q = sqrt(x) */
	r = 0x01000000;		/* r = moving bit from right to left */

	while(r!=0) {
	    t = s+r;
	    if(t<=ix) {
		s    = t+r;
		ix  -= t;
		q   += r;
	    }
	    ix += ix;
	    r>>=1;
	}

    /* use floating add to find out rounding direction */
	if(ix!=0) {
	    z = one-tiny; /* trigger inexact flag */
	    if (z>=one) {
	        z = one+tiny;
		if (z>one)
		    q += 2;
		else
		    q += (q&1);
	    }
	}
	ix = (q>>1)+0x3f000000;
	ix += (m <<23);
	z = fromRep(ix);

	return z;
}

COMPILER_RT_ABI fp_t sqrtf(fp_t a) {
    return __sqrtf(a);
}
