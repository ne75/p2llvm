add_library(
    math OBJECT
    # k_rem_pio2.c
    # k_cos.c
    # k_sin.c
    # k_tan.c
	k_rem_pio2f.c
    k_cosf.c
    k_sinf.c
    # k_tanf.c
	# e_acos.c
    # e_acosh.c
    # e_asin.c
    # e_atan2.c
	# e_acosf.c
    # e_acoshf.c
    # e_asinf.c
    # e_atan2f.c
	# e_atanh.c
    # e_cosh.c
    # e_exp.c
    e_fmod.c
	# e_atanhf.c
    # e_coshf.c
    # e_expf.c
    e_fmodf.c
	# e_hypot.c
    # e_j0.c
    # e_j1.c
    # e_jn.c
    # e_lgamma_r.c
	# e_hypotf.c
    # e_j0f.c
    # e_j1f.c
    # e_jnf.c
    # e_lgammaf_r.c
	# e_log.c
    # e_log10.c
    # e_pow.c
    # e_rem_pio2.c
    # e_remainder.c
	# e_logf.c
    # e_log10f.c
    e_powf.c
    e_rem_pio2f.c
    # e_remainderf.c
	# e_scalb.c
    # e_scalbf.c
    # e_sinh.c
    # e_sinhf.c
    # e_sqrt.c
    # e_sqrtf.c
	# s_asinh.c
    # s_atan.c
    # s_cbrt.c
    # s_ceil.c
    # s_copysign.c
	# s_asinhf.c
    # s_atanf.c
    # s_cbrtf.c
    # s_ceilf.c
    # s_copysignf.c
	# s_cos.c
    # s_erf.c
    # s_expm1.c
    # s_fabs.c
    # s_fmax.c
    # s_fmin.c
    # s_floor.c
	s_cosf.c
    # s_erff.c
    # s_expm1f.c
    # s_fabsf.c
    # s_fmaxf.c
    # s_fminf.c
    s_floorf.c
	# s_frexp.c
    # s_ilogb.c
	# s_frexpf.c
    # s_ilogbf.c
	# s_log1p.c
    # s_logb.c
    # s_modf.c
    # s_nextafter.c
	# s_log1pf.c
    # s_logbf.c
    # s_modff.c
    # s_nextafterf.c
	# s_rint.c
    s_scalbn.c
    # s_signgam.c
    # s_significand.c
    # s_sin.c
	# s_rintf.c
    s_scalbnf.c
    # s_significandf.c
    s_sinf.c
	# s_tan.c
    # s_tanh.c
	# s_tanf.c
    # s_tanhf.c
	# s_cacos.c
    # s_cacosf.c
    # s_casin.c
    # s_casinf.c
    # s_catan.c
    # s_catanf.c
	# s_cacosh.c
    # s_cacoshf.c
    # s_casinh.c
    # s_casinhf.c
    # s_catanh.c
    # s_catanhf.c
	# s_ccos.c
    # s_ccosf.c
    # s_csin.c
    # s_csinf.c
    # s_ctan.c
    # s_ctanf.c
	# s_ccosh.c
    # s_ccoshf.c
    # s_csinh.c
    # s_csinhf.c
    # s_ctanh.c
    # s_ctanhf.c
	# s_cexp.c
    # s_cexpf.c
    # s_clog.c
    # s_clogf.c
    # s_cpow.c
    # s_cpowf.c
	# s_carg.c
    # s_cargf.c
    # s_cimag.c
    # s_cimagf.c
    # s_creal.c
    # s_crealf.c
	# s_conj.c
    # s_conjf.c
    # s_cproj.c
    # s_cprojf.c
	# s_csqrt.c
    # s_csqrtf.c
    # s_cabs.c
    # s_cabsf.c
	# isnan.c
    # k_sincos.c
    # k_sincosf.c
)

target_include_directories(math PRIVATE ${CMAKE_SOURCE_DIR}/include)