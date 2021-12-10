LLVM = /opt/p2llvm
CC = $(LLVM)/bin/clang
LD = $(LLVM)/bin/ld.lld
AR = $(LLVM)/bin/llvm-ar

OBJDIR = build

CFLAGS = -Oz -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -I./include/ #-mllvm -debug

LIBTARGET = libp2
LIBNAME = p2

.PHONY: all setup clean

all: setup $(OBJDIR)/$(LIBTARGET).a

setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/math

gcc-single-routines := negsf2 addsf3 subsf3 mulsf3 divsf3 eqsf2 \
	lesf2 gesf2 unordsf2 fixsfsi fixunssfsi floatsisf fixsfdi        \
	fixunssfdi floatdisf floatunsisf floatundisf

gcc-double-routines := negdf2 adddf3 subdf3 muldf3 divdf3 eqdf2 \
	ledf2 gedf2 unorddf2 fixdfsi fixunsdfsi floatsidf fixdfdi        \
	fixunsdfdi floatdidf extendsfdf2 truncdfsf2 floatunsidf \
	floatundidf

gcc-quad-routines := negtf2 addtf3 subtf3 multf3 divtf3 eqtf2 \
	letf2 getf2 unordtf2 fixtfsi fixunstfsi floatsitf fixtfdi      \
	fixunstfdi floatditf extendsftf2 trunctfsf2 extenddftf2 \
	trunctfdf2 floatunsitf floatunditf

OBJS = crt0.o propeller2.o p2db.o p2db_isr.o \
		math/ashldi3.o math/ashrdi3.o math/divdi3.o math/divsi3.o math/lshrdi3.o math/moddi3.o \
		math/modsi3.o math/muldi3.o math/negdi2.o math/udivdi3.o math/udivmoddi4.o math/umoddi3.o 

$(OBJDIR)/$(LIBTARGET).a: $(addprefix $(OBJDIR)/, $(OBJS))
		$(AR) rc $@ $^

$(OBJDIR)/p2db_isr.o: lib/p2db_isr.c
		$(CC) $(CFLAGS) -falign-functions=128 -o $@ -c $<

$(OBJDIR)/%.o: lib/%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf ./$(OBJDIR)
