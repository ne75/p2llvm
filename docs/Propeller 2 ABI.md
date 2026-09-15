# Propeller 2 Application Binary Interface

## Introduction

Propeller 2 is an multicore 32-bit microcontroller with 64 smartpins, 8 cores (called cogs), and a central hub. For complete details on this processor, see [Propeller 2 Main Product Page](https://www.parallax.com/propeller-2/)

### Document Conventions

- Decimal integers are written without any prefix/postfix, i.e. `1234`
- Hexadecimal integers are written as `0x1234abcd` 

### Reference Documents

[Propeller 2 Datasheet](https://www.parallax.com/package/propeller-2-p2x8c4m64p-datasheet/)

[Propeller 2 Silicon Documentation](https://docs.google.com/document/d/1gn6oaT5Ib7CytvlZHacmrSbVBJsD9t_-kmvjd7nUR6o/edit?usp=sharing)

[Propeller 2 Instruction Set](https://docs.google.com/spreadsheets/d/1_vJk-Ad569UMwgXTKTdfJkHYHpc1rZwxB-DcIiAZNdk/edit?usp=sharing)

## Memory Organization 

### Cog RAM

| Address | Name | Purpose |
|-|-|-|
| `0x000` - `0x1cf` | - | General Purpose Data/Instructions |
| `0x1d0` - `0x1ef` | `r0`-`r31` | General Purpose Data (for use by the compiler)
| `0x1f0` | `ijmp3` | Interrupt 3 ISR address|
| `0x1f1` | `iret3` | Interrupt 3 return address|
| `0x1f2` | `ijmp2` | Interrupt 2 ISR address|
| `0x1f3` | `iret2` | Interrupt 2 return address|
| `0x1f4` | `ijmp1` | Interrupt 1 ISR address|
| `0x1f5` | `iret1` | Interrupt 1 return address|
| `0x1f6` | `pa` | Special call register (used as scratch register) | 
| `0x1f7` | `pb` | Special call register; not callee-saved |
| `0x1f8` | `ptra` | Stack pointer | 
| `0x1f9` | `ptrb` | Scratch pointer; not callee-saved |
| `0x1fa` | `dira` | Pin port A direction control |
| `0x1fb` | `dirb` | Pin port B direction control |
| `0x1fc` | `outa` | Pin port A output control |
| `0x1fd` | `outb` | Pin port B output control |
| `0x1fe` | `ina` | Pin port A input state<sup>1</sup> |
| `0x1ff` | `inb` | Pin port B input state<sup>1</sup> |

*<sup>1</sup>These are also used as ISR/return address for interrupt 0, a high priority debug interrupt*


Each cog contains a 2KB, long-addressed RAM. Each long in RAM is referred to as a "register". Each register can store data or instructions, and instructions reference this RAM to manipulate data (except for instructions that explicitly read/write hub RAM). 

The first 464 registers (`0x000` - `0x1cf`) are general purpose registers and are only used if the application specifically references them. 

The next 32 registers (`0x1d0` - `0x1ef`) are general purpose registers named r0-r31 and are used by the compiler as registers to perform operations on (analogous to a typical processor's register file)

The next 16 registers (`0x1f0` - `0x1ff`) are special-purpose registers that control hardware, interrupt jump/return locations, and the stack pointer.

The cog can execute code stored in the general purpose space by pointing it's PC to the corresponding register address (`0x000` - `0x1cf`). PC can also point to `0x1d0` - `0x1ff`, but that isn't useful. 

### LUT RAM 

Each cog also contains a 2KB, long-addressed "look up table" RAM. This RAM can also store data or instructions, but data can only be accessed via specific read/write instructions and cannot be addressed directly by instructions. However, instructions stored in LUT RAM can be fetched by PC pointing to `0x200` - `0x3ff`.

### Hub RAM

The hub is a 512KB, byte-addressed RAM that can be accessed by all cogs. It is used to store both data and program code. Each cog can fetch it's instructions from the hub directly, using the program counter as the address. Because the PC can point to cog RAM, lut RAM, or hub RAM, a cog cannot execute from hub RAM addresses below `0x400`

## Data Types and Representation

HUB data is little endian. Hardware byte addressing does not imply that C/C++
objects all have one-byte alignment. The frontend's normal type layout is:

### Basic Data Types

| Type | Size (bytes) | C/C++ alignment (bytes) |
|---|---:|---:|
| signed/unsigned char | 1 | 1 |
| signed/unsigned short | 2 | 2 |
| signed/unsigned int, long | 4 | 4 |
| signed/unsigned long long | 8 | 8 |
| float | 4 | 4 |
| double, long double | 8 | 8 |
| object/function pointer | 4 | 4 |

`float` uses IEEE binary32; `double` and `long double` use IEEE binary64.
`size_t` is unsigned long; `ptrdiff_t` is signed long, both 32 bits. `wchar_t`
is signed int. Struct members follow these alignments unless explicitly packed.

The LLVM data layout is `e-p:32:32-i32:32-i64:32`. Its default i64 alignment
is four bytes; Clang supplies the stronger C/C++ object alignment explicitly.
Incoming ABI stack slots are byte aligned regardless of natural type alignment.
The backend reserves alignment slack for local objects and rounds their addresses
up at access time; it does not assume the caller's PTRA is naturally aligned.

### Register Data Storage

Any of the 8, 16, or 32 bit types can be stored in any cog register. The 64 bit types can be stored in even/odd pairs of registers. 

## Program Layout

Hub RAM ranges from `0x00000` to `0x7ffff`. The top 16KB (`0x7c000`-`0x7ffff`) is reserved for the debugger code, if used, and is mapped to via address range `0xfc000`-`0xfffff`. If debugging is not used, `0x7c000`-`0x7ffff` is normal addressable RAM space. 

The supplied `libp2/p2.ld` and `p2_debug.ld` scripts define this layout.

| Address     | Description | 
|-------------|-------------|
| `0x00000`   | One time startup code to jump to re-usable startup code | 
| `0x00040`   | Re-usable cog startup code |
| `0x00200`   | Runtime library code | 
| `0x00a00`   | Start of generic program space |
| `0x70000`   | Start of 48 KiB Cog 0 stack reservation |
| `0x7c000`   | End of stack; start of debugger reserved space |
| `0x7ffff`   | End of memory (on Rev B silicon) |

The scripts reserve `0xc000 - 4` bytes for the heap after BSS, aligned to four
bytes. Other cogs need separately allocated stacks. The scripts enforce a 2 KiB
LUT image limit; this is not a runtime stack overflow check or a measured stack
high-water bound. Stock startup loads LUT from HUB `0x200`, so relocating the
LUT image requires corresponding startup changes.

## Program Execution

### HUB, LUT and COG execution

Ordinary functions execute from HUB. Standard startup copies the shared runtime
image into each cog's LUT. Direct call relocations to input sections `lut`, `.lut`
and `.lut.*` use the LUT execution address, based on the linker's image bounds;
this is section-based and is not limited to compiler-generated libcalls.

Taking a function's address still produces its HUB load address. Do not use a
plain function pointer to enter a LUT-resident function; indirect LUT/COG calls
and arbitrary multi-function COG placement have no supported loading contract.

`cogtext` and `cogcache` are accepted metadata, not automatic placement or loading
requests. A section attribute controls placement. `cogmain` has active entry
semantics: it loads R0 from the HUB long at PTRA and skips ordinary callee saves.
It is for raw cog startup code, not a normally callable function. It does not
create a multi-function resident group or initialize a valid return frame.

Use `cogstart()` for the existing standard bootstrap path. Its worker receives
its parameter in R0 after LUT initialization and must not return; stop the cog
or loop when finished. The low-level `_coginit()` uses the supplied hardware mode,
entry address and parameter directly. See startup details below and the
[future COG work](../development/future-improvements.md).

### Branching
Branches are always executed using the `jmp` instruction. Conditional branches are generated as two instructions: a compare and a jump, as shown below:
```
            cmp rx, ry wcz     ' compare rx to ry, writing c/z
if_<cond>   jmp #jmp_target     ' jump to jmp_target based on the values of c/z
```

In special cases, such as a short distance jump on the condition that a value is 0 or not 0, a single instruction jump can be used:
```
            tjz rx, #jmp_target     ' jump to jmp_target if rx is 0
            tjnz ry, #jmp_target    ' jump to jmp_target if ry is not 0
```

### Calling Convention

The stack grows upward in HUB RAM. PTRA points at the first unused byte outside
an active call setup. The incoming stack has only byte alignment; an assembly
caller must not assume all argument slots or saved registers are naturally aligned.

| Register | Ordinary function contract |
|---|---|
| R0–R3 | Argument registers; also callee-saved |
| R4–R29 | Callee-saved general registers |
| R30, R31 | Return values and caller-clobbered scratch |
| PA, PB, PTRB | Not callee-saved; inline assembly must declare uses/clobbers |
| PTRA | Upward-growing stack pointer; restored on return |
| C/Z and Q/prefix state | Do not rely on preservation across a call |

#### Call sequence and frame

1. The caller writes any stack arguments and increments PTRA by their total size.
2. `CALLA` pushes a four-byte return record and increments PTRA by four.
3. The callee saves any R0–R29 it modifies, then reserves its local frame.
4. The callee produces its result, releases locals, restores saved registers and
   uses `RETA` to pop the return record.
5. The caller releases its outgoing argument area.

From low to high addresses, a frame contains incoming arguments, the return
record, saved registers, and locals/alignment slack. Within the argument area,
arguments run downward: the first stack argument is nearest the return record.
The fixed frame size is not the maximum stack use of nested calls or recursion.

Contiguous saved-register runs use `SETQ` with `WRLONG`/`RDLONG` block transfers.
Separate runs use separate transfers. Assembly helpers must declare their inputs,
outputs and clobbers so the compiler can generate correct saves; writing R30/R31
inside otherwise ordinary C does not declare a C return value.

#### Arguments

Nonvariadic scalar arguments use available R0–R3. Narrow integer values are
promoted to 32-bit register/stack slots. A 64-bit value uses an available even/odd
pair R0/R1 or R2/R3, low word first. A pair cannot straddle an occupied register;
this may leave a single register available for a later 32-bit argument.

Remaining scalar arguments use byte-aligned four- or eight-byte stack slots.
Trivial aggregate arguments are copied inline into the descending stack area
using their full object size. Nontrivial C++ argument/return decisions also follow
Clang's C++ ABI lowering; do not assume every class is a trivial by-value object.

All arguments to a variadic function, including named arguments, use the stack.
`va_list` is a pointer which advances toward lower addresses; `va_arg` subtracts
the requested promoted type's size before reading. Apply C default promotions:
pass `int` for small integers and `double` for `float`. Trivial aggregates occupy
their full inline size rather than a pointer-sized slot.

#### Returns

32-bit integers, pointers and float bit patterns return in R31. Narrow integer
results use the promoted register slot. 64-bit integers and double bit patterns
return low word in R30 and high word in R31.

Aggregate returns use Clang's indirect result convention where applicable: the
caller supplies a hidden result-buffer pointer, and the callee writes the result
there. The backend also returns that pointer in R31 for `sret` functions.

## Propeller 2 Library

### Startup Code

#### `__entry()`

This function is loaded to `0x00000` and is exeuted on boot. It will optionally enable debugging (if the debugging library, `p2db` is linked), and then restart cog 0 at `__start()`

#### `__start()`

`__start()` initializes the runtime LUT. Cog 0 then installs the default stack,
clears BSS, initializes library state, runs constructors, calls `main`, runs
finalizers and stops progressing in `__unreachable()`.

For other cogs, the incoming PTRA points to a caller-owned stack whose first long
contains the worker address and whose second long contains the parameter.
Startup reads those into R1 and R0 and jumps to the worker. `cogstart()` prepares
these two words and starts `__start()` through `_coginit()`. Its `stacksize`
parameter currently does not enforce bounds; the caller owns sufficient storage
and its lifetime. There is no synthetic return address for the worker.

### Function Reference

Below is a list of the functions provided by libp2. See `propeller2.h` for more complete documentation of each function. Eventually move this into better documentation

#### `__unreachable()`
Simple abort function if something goes wrong.

#### `busywait()`
A function to sleep the cog indefinitely. 

#### `__lock_dbg()`
Lock the debug UART lock

#### `__unlock_dbg()`
Unlock the debug UART lock

#### `_clkset()`
Configure the system clock

#### `_cnt()`
Get the current system clock count

#### `_waitcnt()`
Wait until the system clock reaches a value

#### `_coginit()`
Execute hardware COGINIT with the supplied mode, entry address and parameter.

#### `_rev()`
Reverse bits in a long

#### `_uart_init()`
Initialize a UART interface on a pair of smart pins

#### `_uart_putc()`
Write out a character over a previously configured smart pin

#### `_uart_checkc()`
Check if a character is available on a previously configured smart pin

#### `_uart_getc()`
Get a character from a previously configured smart pin

#### `_locknew()`
Get a new lock from the lock pool

#### `_lockret()`
Return a lock to the lock pool

#### `_locktry()`
Try to lock a lock, returning whether or not it was successful

#### `_lock()`
Lock a lock, blocking until it succeeds. 

#### `_unlock()`
Unlock a lock 

#### `cogstart()`
Start a worker through the standard `__start()` bootstrap and initialize its LUT.

## C Standard Library