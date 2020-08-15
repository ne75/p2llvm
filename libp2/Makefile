LLVM = /opt/p2llvm
CC = $(LLVM)/bin/clang
LD = $(LLVM)/bin/ld.lld
AR = $(LLVM)/bin/llvm-ar

OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -I./include/

LIBTARGET = libp2
LIBNAME = p2

.PHONY: all setup clean

all: setup $(OBJDIR)/$(LIBTARGET).a

setup:
	mkdir -p $(OBJDIR)

OBJS = crt0.o propeller2.o printf.o

$(OBJDIR)/$(LIBTARGET).a: $(addprefix $(OBJDIR)/, $(OBJS))
		$(AR) rcs $@ $^

$(OBJDIR)/%.o: lib/%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf ./$(OBJDIR)