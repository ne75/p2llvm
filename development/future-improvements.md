# Future improvements

## Multi-function COG residency

Planned after the phase-0 cleanup and validation work. Implementation has not
started.

- Mark functions as COG-resident and call them from HUB code on the current cog.
- Start a cog whose main function can call other functions resident in that
  cog's RAM. Evaluate a shared HUB bootstrap that loads a resident function
  group and calls its COG-resident main entry.

The design needs a way to declare resident groups, linker placement and capacity
checks, distinct HUB load and COG execution addresses, and a runtime loading
contract for each cog. Define HUB/COG calls and nested returns without violating
register or stack preservation. Existing `cogmain`/`cogtext` behavior needs a
compatibility review as part of that design.

Acceptance requires hardware tests at O0/O2/Os for HUB-to-COG calls, calls between
resident functions, returns to HUB code, and independent residency in multiple
cogs. Oversized groups must receive a clear diagnostic. Measure call/loading
costs and code size as well as checking results and ABI preservation.
