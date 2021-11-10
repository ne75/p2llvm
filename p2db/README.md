# p2db

A python-based debugger for Propeller 2

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

Each command has the following format: `$<command><value>` in binary format. Value is always 4 bytes long, so each command string is 6 bytes long
- `command` is a single byte for what the code should do
    - `b`: execute `brk <value>` 
    - `r`: get value at ram address <value>
    - `h`: get value at hub address <value>

Each response has the following format: `~<code><values>`
- `code` is what values contains
    - `g`: 3 longs with the values from getbrk
    - `r`: 1 long from cog or lut ram
    - `h`: 1 byte from hub ram

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