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
| `0x1d0` - `0x1f0` | `r0`-`r31` | General Purpose Data (for use by the compiler)
| `0x1f0` | `ijmp3` | Interrupt 3 ISR address|
| `0x1f1` | `iret3` | Interrupt 3 return address|
| `0x1f2` | `ijmp2` | Interrupt 2 ISR address|
| `0x1f3` | `iret2` | Interrupt 2 return address|
| `0x1f4` | `ijmp1` | Interrupt 1 ISR address|
| `0x1f5` | `ijmp1` | Interrupt 1 return address|
| `0x1f6` | `pa` | Special call register (used as scratch register) | 
| `0x1f7` | `pb` | Special call register (unused) |
| `0x1f8` | `ptra` | Stack pointer | 
| `0x1f9` | `ptrb` | Ununsed |
| `0x1fa` | `dira` | Pin port A direction control |
| `0x1fb` | `dirb` | Pin port B direction control |
| `0x1fc` | `outa` | Pin port A output control |
| `0x1fd` | `outb` | Pin port B output control |
| `0x1fe` | `ina` | Pin port A input state<sup>1</sup> |
| `0x1ff` | `inb` | Pin port B input state<sup>1</sup> |

*<sup>1</sup>These are also used as ISR/return address for interrupt 0, a high priority debug interrupt*


Each cog contains a 2KB, long-addressed RAM. Each long in RAM is referred to as a "register". Each register can store data or instructions, and instructions reference this RAM to manipulate data (except for instructions that explicitly read/write hub RAM). 

The first 464 registers (`0x000` - `0x1cf`) are general purpose registers and are only used if the application specifically references them. 

The next 32 registers (`0x1d0` - `0x1f0`) are general purpose registers named r0-r31 and are used by the compiler as registers to perform operations on (analogous to a typical processor's register file)

The next 16 registers (`0x1f0` - `0x1ff`) are special-purpose registers that control hardware, interrupt jump/return locations, and the stack pointer.

The cog can execute code stored in the general purpose space by pointing it's PC to the corresponding register address (`0x000` - `0x1cf`). PC can also point to `0x1d0` - `0x1ff`, but that isn't useful. 

### LUT RAM 

Each cog also contains a 2KB, long-addressed "look up table" RAM. This RAM can also store data or instructions, but data can only be accessed via specific read/write instructions and cannot be addressed directly by instructions. However, instructions stored in LUT RAM can be fetched by PC pointing to `0x200` - `0x3ff`.

### Hub RAM

The hub is a 512KB, byte-addressed RAM that can be accessed by all cogs. It is used to store both data and program code. Each cog can fetch it's instructions from the hub directly, using the program counter as the address. Because the PC can point to cog RAM, lut RAM, or hub RAM, a cog cannot execute from hub RAM addresses below `0x400`

## Data Types and Representation

Data in the hub is always byte-aligned and stored with least significant byte  first (little endian format). 

### Basic Data Types

| Type | Propeller-style Name |  Size (bits) | Hub Alignment (bits) |
|-|-|-|-|
| signed/unsigned char | byte | 8 | 8 | 
| signed/unsigned short | word | 16 | 8 |
| signed/unsigned int | long | 32 | 8 |
| signed/unsigned long long | - | 64 | 8 | 
| float | - | 32 | 8 | 
| double | - | 64 | 8 |

### Register Data Storage

Any of the 8, 16, or 32 bit types can be stored in any cog register. The 64 bit types can be stored in even/odd pairs of registers. 

## Program Layout

Hub RAM ranges from `0x00000` to `0x7ffff`. The top 16KB (`0x7c000`-`0x7ffff`) is reserved for the debugger code, if used, and is mapped to via address range `0xfc000`-`0xfffff`. If debugging is not used, `0x7c000`-`0x7ffff` is normal addressable RAM space. 

The following table describes how a typical program will be laid out.

| Address     | Description | 
|-------------|-------------|
| `0x00000`   | One time startup code to jump to re-usable startup code | 
| `0x00040`   | Re-usable cog startup code |
| `0x00200`   | Runtime library code | 
| `0x00a00`   | Start of generic program space |
| `0x70000`   | 2KB Cog 0 Stack Space |
| `0x7cfff`   | Start of debugger reserved space |
| `0x7ffff`   | End of memory (on Rev B silicon) |

## Program Execution

### Cogex and Hubex Modes
By default, code executs in Hubex mode (PC = `0x400` or greater). This is slower, but a lot more flexible for code space. However, several builtin functions are stored in cog LUT RAM, and are re-loaded into each new cog that gets started as a "standard" cog. Cogs can also be started with arbitrary data to begin executing. This can be a function compiled by the compiler, an externally linked binary, or binary blob compiled in with the program. For the first option, the function should be marked with the `cogtext` attribute. (Side note: this doesn't actually do anything right now, but the attribute is made available for future use)

In general, code will execute in hubex mode, since all code is stored in the hub. Some functions (specifically the Propeller runtime library functions), can be placed into cog LUT RAM and be executed from there. Currently, only runtime libcalls generated by the compiler (for instructions that don't have matching Propeller instructions or simple instruction groups) can be placed into cog LUT RAM. Anything else won't relocate correctly, so function calls will jump to their hub RAM locations instead of the proper cog RAM locations.

Execution is performed similar to a classic processor with a register file and separate memory for data. This layout was described in *Memory Layout*. A stack pointer keeps track of a stack in memory that is used for local variables. `ptra` is used as the stack pointer and its value gets set up in the cog startup function, `__start()`. In general, the start of a function will allocate the stack space it needs by adding to `ptra`, with the stack growing **up**. There are 2 places where `ptra` gets adjusted automatically: via a function call, which will push PC and the status word to the stack and increment `ptra`, and when functions save registers to the stack in the prologue/restore them in the epilogue. This is described in detail in *Calling Convention* above. Generally, `ptra` always points to first empty stack slot. The only exception to this is during a call sequence, where the stack is written to before the pointer is actually incremented. 

### Branching
Branches are always executed using the `jmp` instruction. Conditional branches are generated as two instructions: a compare and a jump, as shown below:
```
            cmp rx, ry, wcz     ' compare rx to ry, writing c/z
if_<cond>   jmp #jmp_target     ' jump to jmp_target based on the values of c/z
```

In special cases, such as a short distance jump on the condition that a value is 0 or not 0, a single instruction jump can be used:
```
            tjz rx, #jmp_target     ' jump to jmp_target if rx is 0
            tjnz ry, #jmp_target    ' jump to jmp_target if ry is not 0
```

### Calling Convention

Data for functions is paseed in and out via two methods: registers or the stack. Various types of data will prefer one or the other. Any function call follows the following process:
1. The **caller** will allocate space on the stack (if needed), write the arguments to the stack (if needed). The stack pointer is incremented after all the writing, right before the actual call. This is so that if one of the arguments is a pointer to something in the caller's stack frame, the correct value is used. 
1. The **caller** will call the function using `calla`. This saves the built-in status word to `ptra` and auto-increments `ptra`
1. The **callee** will push any registers it needs to save to the stack using block transfers (described below). This will also auto-increment `ptra`
1. The **callee** will allocate any stack space it needs for it's local variables that won't fit in registers, local arrays, etc.
1. The **callee** will read in any stack-based arguments (if needed).
1. The **callee** executes any code necessary, potentially calling more functions
1. The **callee** will set `r30` and `r31` for return values, (if any)
1. The **callee** will pop any registers it saved using block transfers
1. The **callee** will return using `reta`. This will auto-decrement `ptra` and restore PC and the status word to their values before the call.
1. The **caller** will de-allocate the stack allocated before the call.

#### Call Stack Frame:

The stack will grow up. ptra always points to the TOP of the stack, which is the start of free stack space. Selection will generate FRMIDX pseudo instructions that will be lowered in register info by be subtracting from the current stack pointer (sp) by the frame index offset. The callee will save any regsiters it uses. The data in the stack frame will be organized as follows:

```
SP ----------> ----------------- (4)
                local variables
                ...
               ----------------- (3)
                callee saved regs
                ...
               ----------------- (2)
                return address (pushed automatically)
               ----------------- (1)
                arguments into function (descending).
                formal arguments come first (highest in the stack)
                followed by variable argument (last var arg is lowest in the stack)
                ...
SP (previous)  -----------------
```

Here's the ordering of sp adjustment: when calling, SP (previous) is adjusted for arguments (1). The function is called and return address (and status word) is pushed onto the stack with the call (2). The function then allocates space it needs to save registers (3), and local variables (4). SP now becomes SP (previous) when getting ready to call another function.

Callee saved register spilling/restoring is done via setq and wrlong/rdlong to do a block transfer of registers to memory. We count up the number of continuous registers that need to be saved in a single setq/wrlong pair. Restoring does the same thing in reverse.


#### Outgoing Function Arguments

`r0` through `r3` are used to pass single values into functions. This was chosen fairly arbitrarily and if there is a performance boost from increasing to more registers, that can be very easily done. 8 and 16 bit values are promoted to 32 bits. 64-bit values are stored in even/odd register pairs (`r0_r1` and `r2_r3`). 

Structs/ByVals are passed via the stack in descending order. 

Variable argument function argument lists are passed exclusively via the stack (even if they are all single values). They are stored in descending order by the caller. This allows the callee to continue to offset downwards from the top of its incoming argument frame until the function logic determines all arguments have been read out. This is necessary since the callee doesn't know how far to offset its stack pointer, since it has no knowledge of how many arguments were passed.

#### Return Values

Values are generally returned via `r31`. `r30` is also reserved for return values if 64-bits are to be returned, instead of using the stack. Structs/ByVals should use the stack; often the caller will allocate stack space for the result and pass a pointer for the callee to store the result into. This decision is made by `clang`

#### Callee-Saved Registers

The callee will save registers using fast block transfers via `setq` and `rd/wrlong` to `ptra`. This will also auto-increment `ptra`. It's important to note that each continuous block of registers will be written separately, so if the registers to save are `r0`, `r1`, `r2`, and `r4`, then the result to push these to the stack will be

```
setq #2
wrlong r0, #351 // 351 is the special immediate for writing to PTRA with post-increment
wrlong r4, #351
```

This means that if the registers are highly discontinuous, we won't take advantage of block transfers and just end up doing individual writes. 

Restoring CSRs does the exact same thing, just in reverse using `rdlong`.

## Propeller 2 Library

### Startup Code

#### `__entry()`

This function is loaded to `0x00000` and is exeuted on boot. It will optionally enable debugging (if the debugging library, `p2db` is linked), and then restart cog 0 at `__start()`

#### `__start()`

This is the startup function that can initiate a new cog. It relies on `setq` being used to set the start of the stack for the new cog. Cog 0 skips this setup and uses the static values for the stack and `main()`, as defined by the linker. Upon startup, the cog startup code assumes `ptra` will point to the start of the stack allocated for this cog, with the first long in the stack (`ptra[0]`) containing a pointer to the function to begin executing, and the second long in the stack (`ptra[1]`) containing an optional parameter to be passed into the new function, via `r0`

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
Initialize a new hubex cog by using `__start()`

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
Start a new native (cogex) cog

## C Standard Library