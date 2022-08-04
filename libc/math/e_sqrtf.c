/* e_sqrtf.c -- float version of e_sqrt.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#include "math.h"
#include "math_private.h"

#include <stdio.h>

float sqrtf(float x) {
	// float z;
	// int32_t sign = (int)0x80000000; 
	// int32_t ix;

	// GET_FLOAT_WORD(ix,x);

    // // /* take care of Inf and NaN */
	// // if((ix&0x7f800000)==0x7f800000) {			
	// //     return x*x+x;		/* sqrt(NaN)=NaN, sqrt(+inf)=+inf
	// // 				   sqrt(-inf)=sNaN */
	// // } 
    // // /* take care of zero */
	// // if(ix<=0) {
	// //     if((ix&(~sign))==0) return x;/* sqrt(+-0) = +-0 */
	// //     else if(ix<0)
	// // 	return (x-x)/(x-x);		/* sqrt(-ve) = sNaN */
	// // }

	// int32_t d = (int)x;
	// u_int32_t s = (int)(x * 4294967296.0f);

	// printf("%d, %d", d, s);

	// SET_FLOAT_WORD(z,ix);
	return 0;
}