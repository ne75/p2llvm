# p2db

A python-based debugger for Propeller 2

## Disclaimer

This is a very rough debugger meant to be very simple tool for debugging basic issues. It has bugs, not all corner cases are tested, and doesn't have the greatest UX. Sorry. 

## Requirements

- p2llvm is installed to `/opt/p2llvm`
- pyserial
- colorama
- blessed

## Usage

Cog 0 debugging is always enabled by libp2. The debug ISR will start once and immediate return since it was a coginit interrupt. In the main application, add a `brk(x)` to trigger entering the debug ISR. `x` can be any value, it is not used the by ISR. To enable debugging in other cogs, use `hubset(DEBUG_INT_EN | DEBUG_COGx)` where x is the desired cog. On startup, the target cog needs to call `brk` to re-enter the debug ISR for that cog. 

To debug the application, install the Python package with `pip install .` from the top level `p2db/` directory. Run the debugger, given the port and application:

```p2db <port> <app>```

- `<port>` is the port with the attached P2. 
- `<app>` is the path to the ELF application to load and debug

## Comands

Type `help` for a list of commands. Type `help <command>` for help with a specific command. They are also summarized below

- `step` or `s`: Step a single instructions. Executes `brk 1` on the P2 side. If the instruction to be executed is a function call, this command will instead break on the instruction following the function call
- `stepin` or `i`: Step into a function call. Allows you to single step through a function call instead of stepping over it. 
- `break <addr>`: Break on a specific address. 
- `getbyte <addr>`: get the byte at hub RAM address `addr`. Must be in hex (the leading `0x` can be dropped, i.e. `getbyte 123` is okay).
- `getlong <addr>`: get the 4-byte long at hub RAM address `addr`. Must be in hex. 
- `getreg <addr>|<name>`: get the value in the cog RAM register given by either an address (in hex) or a named register (one of the 16 special function registers, or r0-r31). the value returned is the value that was spilled by the debug ISR, NOT the true value in the register when in the debug ISR. This is so that you see what the actual application is operating on, not just the debug ISR's values. 
- `cog <n>`: Change view to cog `<n>`. 
- `cogaddr <addr>`: When debugging a native cog (i.e. one that loaded a function directly using `_coginit()`, rather than running from the hub using `cogstart()`), the debugger has no way of knowing where in the actual elf binary that function is located. Use this command to explicitly say which address the cog was loaded from so that instruction viewing is possible. 

## Principle of Operation

These are notes on what I will implement, formal docs to come

### P2 Side

1. Debugging must be enabled
    - `hubset 0x2001ffff`: enable debug interrupts on all cogs, and write protect the debug ROM space
    - must be called after _uart_init
2. The small 16-instruction debug program (`__dbg_isr#`, where `# is the cog number`) is loaded 8 times, starting at 0xFFC40. Need to have 1 image for every cog, since each cog loads from a different address
3. The small program does the following
    - save ram registers 0xf0..0x1ef to 0xfc000 + n*0x400, where n is the cog number
    - load the larger debug program from 0xfe000 to 0xf0
    - run the actual program
    - restore ram regsiters 0xf0..0x1ef
    - jumpe to the built-in exit routine 
4. The main program does the following
    - dump all current state info over serial 
    - read in the next debugger command over serial
    - set the debugger command
    - return to the small isr

### Host Side

1. Use loadp2 to load the application and the debug image to RAM
2. Connect over serial, wait for debug data to come ing
3. Use some kind of prompt to allow the user to send commands 

## Protocol 

Each command has the following format: `\xdb<cog><command><value>` in binary format. Value is always 4 bytes long, so each command string is 7 bytes long
- `command` is a single byte for what the code should do
    - `b`: execute `brk <value>` 
    - `r`: get value at ram address <value>
    - `h`: get value at hub address <value>
    - 's': get status of the cog. <value> can be anything. This will also force the the given cog to take control of the debug isr 

Each response has the following format: `\xdb<code><values>`
- `code` is what values contains
    - `g`: 3 longs with the values from getbrk
    - `r`: 1 long from cog or lut ram
    - `h`: 1 byte from hub ram

A non-ascii start character is used so that printf statements that might be using the same serial pipe are not confused for debug messages

## The main debug image

The primary debugger program needs to fit in 256 instructions and should do the following:

1. On entry, `getbrk` all the current status bytes
1. If this was the result of a coginit, enable teh brk interrupt condition and return. If not coginit: 
1. Send status bytes over serial (`g`)
1. Wait for a serial command to come in. 
1. Execute the command
    - if the command is a b (which means set up the next breakpoint), continue to the `exit` routine, else, wait for another command

## Multicog support

To make this work with multiple cogs, I think I'll do the following: 

1. ~~change all commands to include a cog number before the command byte. This will be the cog the command is destined to.~~
1. ~~change all responses to include which cog is responding, after the response code.~~
1. ~~in the __entry() routine of crt0, create a debug lock.~~ 
1. ~~on entry to the debug ISR, lock the debug lock. this prevents any other cog from using debug space. ~~
1. ~~check for new data~~
    - check if there's anything in the rx buffer (by checking the first byte being $). If there is, process that command.
    - if empty, try reading in a command
1. when a cog processes a command, check which cog the command was for. 
    - If it was for this cog, process it and respond
    - If it was for a different cog, continue
1. on exit when a command is succesfully processed, clear the first byte of the rx buffer to indicate there is nothing for other cogs to do.
1. unlock the debug lock and return. 