#
# Makefile for Propeller C library
# This is the fragment that will build just one model
# Invoke it like "make -f Makefile.incl MODEL=lmm
#

ifndef OBJDIR
	OBJDIR=./build
endif

ifndef DEST
	DEST = ~/Documents/Programming/Propeller/p2-llvm
endif

# ifeq ($(SHORTDOUBLES),1)
# MODELDIR=$(OBJDIR)/$(MODEL)$(CHIPDIR)/short-doubles
# FLOATMODEL=-m32bit-doubles
# FLOATDEF=-D__SHORT_DOUBLES_IMPL
# else
# MODELDIR=$(OBJDIR)/$(MODEL)$(CHIPDIR)
# FLOATMODEL=-m64bit-doubles
# FLOATDEF=
# endif
# MATHDIR=$(MODELDIR)/libm_obj

PREFIX = $(OBJDIR)
WARN = -Wall -Werror
#WARN =
BASEFLAGS = $(WARN) -Os -nostdinc -I./include -ffunction-sections -fno-jump-tables --target=p2
CFLAGS = $(BASEFLAGS) -D__propeller2__
MATHCFLAGS = $(WARN) -m$(MODEL) $(CHIPOPT) $(FLOATDEF) -Os -nostdinc -I./include
#LLVM = ~/Documents/Programming/Propeller/p2-llvm
LLVM = ~/Github/llvm-propeller2/build
AR = $(LLVM)/bin/llvm-ar
CC = $(LLVM)/bin/clang
OBJCOPY = $(LLVM)/bin/llvm-objcopy
MKDIR = mkdir -p

VPATH=.:misc:stdio:stdlib:string:sys:sys/propeller:time:drivers:math:pthreads:wchar

all: $(PREFIX) $(PREFIX)/libc.a

# $(PRINTF) are the functions which depend on vfprintf;
# we include floating point versions of these in the math library
PRINTF = printf.o fprintf.o vprintf.o sprintf.o snprintf.o vsprintf.o vsnprintf.o
SCANF =  scanf.o vscanf.o sscanf.o fscanf.o

STDIO = clearerr.o feof.o ferror.o fflush.o \
	fgetc.o fputc.o fputs.o puts.o putchar.o getchar.o \
	putc.o getc.o ungetc.o gets.o fgets.o \
	setvbuf.o setbuf.o setlinebuf.o setbuffer.o \
	fopen.o fopen_intern.o freopen.o \
	fwrite.o fread.o fseek.o ftell.o fgetpos.o fsetpos.o rewind.o \
	stringio.o vfprintf.o vfscanf.o \
	$(PRINTF) $(SCANF) \
	simple_printf.o \
	default_io.o init_io.o remove.o fmemopen.o \
	tmpnam.o tmpfile.o

STDLIB = malloc.o calloc.o realloc.o atoi.o atoll.o atof.o atexit.o abort.o \
	qsort.o bsearch.o strtol.o strtoul.o strtod.o strtof.o \
	strtoll.o strtoull.o \
	assert.o rand.o div.o lldiv.o abs.o llabs.o getenv.o \
	multibyte.o fenv.o mblencur.o

MISC = 	ctype.o toupper.o tolower.o \
	isalpha.o isalnum.o iscntrl.o isprint.o isspace.o isdigit.o \
        isblank.o isxdigit.o isupper.o islower.o isgraph.o ispunct.o \
	sbrk.o signal.o assert.o locale.o \
	chdir.o rmdir.o mkdir.o opendir.o rdwr.o lseek.o \
	putenv.o fdopen.o open.o close.o unlink.o stat.o isatty.o \
	mktemp.o \
	tinyomp.o

WCHAR = wctype.o iswctype.o iswalnum.o iswalpha.o iswblank.o \
      iswcntrl.o iswdigit.o iswgraph.o iswlower.o iswprint.o \
      iswpunct.o iswspace.o iswupper.o iswxdigit.o towlower.o \
      wcscat.o wcscmp.o wcscpy.o wcslen.o wcschr.o wcsrchr.o \
      wcsncat.o wcsncmp.o wcsncpy.o wmemcpy.o wmemmove.o wmemcmp.o \
      wcstol.o wcstoul.o wcstoll.o wcstoull.o \
      mbrtowc_utf.o mbrtowc_ascii.o wcrtomb_utf.o wcrtomb_ascii.o \
      mbsrtowcs.o mbsinit.o mbrlen.o btowc.o wctob.o \
      wcscoll.o wcsxfrm.o

STRING = memcpy.o memmove.o memset.o memchr.o memcmp.o \
	strcat.o strcmp.o strcpy.o strlen.o \
	strncat.o strncmp.o strncpy.o \
	strchr.o strrchr.o strspn.o strcspn.o strtok.o \
	strcoll.o strxfrm.o \
	strstr.o strpbrk.o \
	strerror.o strcasecmp.o strncasecmp.o xmmcopy.o \
	strdup.o

TIME = clock.o sleep.o usleep.o time.o difftime.o strftime.o localtim.o asctime.o \
	settimeofday.o gettimeofday.o

THREADS = thread.o cogthread.o cogstart.o cogload.o clonecog.o nap.o atomic64.o

PTHREADS = pthread_create.o pthread_join.o pthread_detach.o \
	pthread_mutex.o pthread_attr.o pthread_io.o pthread_cond.o \
	pthread_key.o semaphore.o

DRIVERS = SimpleSerial.o serialparam.o serial_break.o serial_exit.o memory.o \
	FdSerial.o FullDuplexSerial.cog null.o drivers.o rtc.o terminal.o \
	FileDriver.o mount.o dosfs.o file_io.o load_sd_driver.o sd_driver.cog \
	rtc_cog.o i2c_simple.o \
	i2c_interface.o i2c_common.o \
	boot_read.o boot_write.o \
	i2c_driver.cog \
	i2c_driver_boot.cog \
	eeprom.o

LIBM = 	k_rem_pio2.o k_cos.o k_sin.o k_tan.o \
	k_rem_pio2f.o k_cosf.o k_sinf.o k_tanf.o \
	e_acos.o e_acosh.o e_asin.o e_atan2.o \
	e_acosf.o e_acoshf.o e_asinf.o e_atan2f.o \
	e_atanh.o e_cosh.o e_exp.o e_fmod.o \
	e_atanhf.o e_coshf.o e_expf.o e_fmodf.o \
	e_hypot.o e_j0.o e_j1.o e_jn.o e_lgamma_r.o \
	e_hypotf.o e_j0f.o e_j1f.o e_jnf.o e_lgammaf_r.o \
	e_log.o e_log10.o e_pow.o e_rem_pio2.o e_remainder.o  \
	e_logf.o e_log10f.o e_powf.o e_rem_pio2f.o e_remainderf.o  \
	e_scalb.o e_scalbf.o e_sinh.o e_sinhf.o e_sqrt.o e_sqrtf.o \
	s_asinh.o s_atan.o s_cbrt.o s_ceil.o s_copysign.o \
	s_asinhf.o s_atanf.o s_cbrtf.o s_ceilf.o s_copysignf.o \
	s_cos.o s_erf.o s_expm1.o s_fabs.o s_fmax.o s_fmin.o s_floor.o \
	s_cosf.o s_erff.o s_expm1f.o s_fabsf.o s_fmaxf.o s_fminf.o s_floorf.o \
	s_frexp.o s_ilogb.o \
	s_frexpf.o s_ilogbf.o \
	s_log1p.o s_logb.o s_modf.o s_nextafter.o \
	s_log1pf.o s_logbf.o s_modff.o s_nextafterf.o \
	s_rint.o s_scalbn.o s_signgam.o s_significand.o s_sin.o \
	s_rintf.o s_scalbnf.o s_significandf.o s_sinf.o \
	s_tan.o s_tanh.o \
	s_tanf.o s_tanhf.o \
	s_cacos.o s_cacosf.o s_casin.o s_casinf.o s_catan.o s_catanf.o \
	s_cacosh.o s_cacoshf.o s_casinh.o s_casinhf.o s_catanh.o s_catanhf.o \
	s_ccos.o s_ccosf.o s_csin.o s_csinf.o s_ctan.o s_ctanf.o \
	s_ccosh.o s_ccoshf.o s_csinh.o s_csinhf.o s_ctanh.o s_ctanhf.o \
	s_cexp.o s_cexpf.o s_clog.o s_clogf.o s_cpow.o s_cpowf.o \
	s_carg.o s_cargf.o s_cimag.o s_cimagf.o s_creal.o s_crealf.o \
	s_conj.o s_conjf.o s_cproj.o s_cprojf.o \
	s_csqrt.o s_csqrtf.o s_cabs.o s_cabsf.o \
	isnan.o k_sincos.o k_sincosf.o


LIBM_EXTRA =	float_printf.o float_simple_printf.o $(PRINTF) \
	   	float_scanf.o $(SCANF)

# Don't build propeller.c now since the only function in it is now a macro
#PROPELLER = propeller.o

#OBJS = $(STDIO) $(STDLIB) $(MISC) $(THREADS) $(STRING) $(TIME) $(DRIVERS) $(PROPELLER) $(WCHAR)
OBJS = $(STDLIB)

MODEL_OBJS = $(addprefix $(PREFIX)/, $(OBJS))
MODEL_LIBM_OBJS = $(addprefix $(MATHDIR)/, $(LIBM))
MODEL_LIBM_EXTRA_OBJS = $(addprefix $(PREFIX)/, $(LIBM_EXTRA))
MODEL_PTHREAD_OBJS = $(addprefix $(PREFIX)/, $(PTHREADS))

MODEL_MATH_OBJS = $(MODEL_LIBM_OBJS) $(MODEL_LIBM_EXTRA_OBJS)

$(PREFIX)/libc.a: $(MODEL_OBJS)
	$(AR) rs $@ $^

$(PREFIX)/libm.a: $(MODEL_MATH_OBJS)
	$(AR) rs $@ $^

$(PREFIX)/libpthread.a: $(MODEL_PTHREAD_OBJS)
	$(AR) rs $@ $^

$(PREFIX)/float_simple_printf.o: simple_printf.c
	$(CC) $(CFLAGS) -o $@ -DFLOAT_SUPPORT -c $<

$(PREFIX)/float_printf.o: vfprintf.c
	$(CC) $(CFLAGS) -o $@ -DFLOAT_SUPPORT -c $<

$(PREFIX)/float_scanf.o: vfscanf.c
	$(CC) $(CFLAGS) -o $@ -DFLOAT_SUPPORT -c $<

$(PREFIX)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< #-mllvm --debug-only=p2-inst-info #-mllvm -view-isel-dags

# this is used to build the COG C EEPROM driver and is rounded up to 2K bytes so the space can be used as a buffer
# for loading other COGs once the EEPROM driver has been loaded
# size is 496*4=1984 bytes

#$(PREFIX)/%_boot.cog: %.c
#	$(CC) $(BASEFLAGS) -DPARALLAX_I2C_BUS -r -mcog -Wl,--defsym,__$(basename $(notdir $@))=1 -o $@ $<
#	$(OBJCOPY) --keep-global-symbol __$(basename $(notdir $@)) --rename-section .text=$(notdir $@) --pad-to 1984 $@

#$(PREFIX)/%.cog: %.c
#	$(CC) $(BASEFLAGS) -r -mcog -Wl,--defsym,__$(basename $(notdir $@))=1 -o $@ $<
#	$(OBJCOPY) --keep-global-symbol __$(basename $(notdir $@)) --rename-section .text=$(notdir $@) $@

$(PREFIX)/%_boot.o: %.c
	$(CC) $(BASEFLAGS) -DPARALLAX_I2C_BUS -mcog -o $@ $<
	$(OBJCOPY) $@

$(PREFIX)/%_firmware.o: %.c
	$(CC) $(BASEFLAGS) -mcog -o $@ $<

$(PREFIX)/%_firmware.o: %.S
	$(CC) $(BASEFLAGS) -mcog -nostdlib -o $@ $<

$(PREFIX)/%_firmware.o: %.sx
	$(CC) $(BASEFLAGS) -mcog -nostdlib -o $@ $<

$(PREFIX)/%_boot.dat: $(PREFIX)/%_boot.o
	$(OBJCOPY) -O binary -j .text $< $@

$(PREFIX)/%.dat: $(PREFIX)/%_firmware.o
	$(OBJCOPY) -O binary -j .text $< $@

$(PREFIX)/%_boot.cog: $(PREFIX)/%_boot.dat
	cd $(PREFIX); $(OBJCOPY) -I binary -B propeller -O $(CC) --rename-section .data=.hub --pad-to 0x7c0 $(notdir $<) $(notdir $@)

$(PREFIX)/%.cog: $(PREFIX)/%.dat
	cd $(PREFIX); $(OBJCOPY) -I binary -B propeller -O $(CC) --rename-section .data=.text $(notdir $<) $(notdir $@)

$(PREFIX)/%.ecog: %.c
	$(CC) $(BASEFLAGS) -r -mcog -Wl,--defsym,__$(basename $(notdir $@))=1 -o $@ $<
	$(OBJCOPY) --keep-global-symbol __$(basename $(notdir $@)) --rename-section .text=$(notdir $@) --redefine-sym $@

$(PREFIX)/%.o: %.s
	$(CC) $(CFLAGS) -o $@ -c $<

$(PREFIX)/%.o: %.S
	$(CC) $(CFLAGS) -o $@ -c $<

$(PREFIX)/%.o: %.sx
	$(CC) $(CFLAGS) -o $@ -c $<

$(MATHDIR)/%.o: %.c
	$(CC) $(MATHCFLAGS) -o $@ -c $<

SRCLIBS = libc.a libm.a libpthread.a

install: all install-dirs install-includes
	cp $(addprefix $(OBJDIR)/, $(SRCLIBS)) $(DEST)/lib/

# 	cp LIB_LICENSE.txt $(DEST)/lib/
# 	cp ../gcc/COPYING3 ../gcc/COPYING.RUNTIME $(DEST)/lib/
# 	cp $(OBJDIR)/cog/crt0_cog.o $(OBJDIR)/cog/crtend_cog.o $(DEST)/lib/
# 	cp $(OBJDIR)/cog/libcog.a $(DEST)/lib/
# 	cp $(OBJDIR)/p2/cog/crt0_cog2.o $(OBJDIR)/p2/cog/crtend_cog2.o $(DEST)/lib/
# 	cp $(OBJDIR)/p2/cog/libcog2.a $(DEST)/lib/

install-dirs:
	mkdir -p $(DEST)/lib/

# LIBSPECS = libgomp.spec

# install-specs:
# 	cp $(LIBSPECS) $(DEST)/lib/

install-includes:
	cp -r include $(DEST)

clean:
	rm -rf $(OBJDIR) *.a

$(PREFIX):
	$(MKDIR) $@

$(MATHDIR):
	$(MKDIR) $@
