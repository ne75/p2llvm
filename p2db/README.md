# p2db

A python-based debugger for Propeller 2

## Principle of Operation

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
    - `e`: exit debugging. <value> should be 0
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