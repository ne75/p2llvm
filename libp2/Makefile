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

OBJS = crt0.o propeller2.o p2db.o p2db_isr.o

$(OBJDIR)/$(LIBTARGET).a: $(addprefix $(OBJDIR)/, $(OBJS))
		$(AR) rc $@ $^

$(OBJDIR)/p2db_isr.o: lib/p2db_isr.c
		$(CC) $(CFLAGS) -falign-functions=128 -o $@ -c $<

$(OBJDIR)/%.o: lib/%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf ./$(OBJDIR)
