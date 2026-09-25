# Future improvements

## Remaining instruction execution coverage

Deferred until after the cleanup merge. The current inventory has 74 instruction
records without executable fixtures. These are coverage gaps, not a request to
implement or claim validation of every peripheral instruction in this series.

Complete SEUSSF/SEUSSR, attention/pattern events, GPIO and smart pins, streamer,
interrupt and debugging fixtures with independent expected results. Define board
and wiring profiles for peripheral cases. Run new fixtures on hardware and keep
strict coverage inventory checks available; full coverage is not a cleanup merge
gate.

## Performance and stack follow-up

Optimize conservative local-frame alignment and branch expansion after measuring
against the saved production baseline. Add stack high-water observations and
independent repeated-boot timing measurements. Preserve the existing benchmark
results as the comparison point.

## Explicit double-use policy

Keep accidental double use visible. Existing Clang conversion warnings cover
some implicit promotions, but a comprehensive target-specific implicit-double
warning and opt-in runtime policy need a separate design. Do not treat adding
conversion helpers as approval to enable implicit doubles in production builds.

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

## Global boolean load instruction selection

The runtime-lock regression exposed a remaining i1 global-load selection failure
at O2/Os (an any-extending load from a global address). Register-addressed i1
coverage does not cover this form. The following public reproducer fails with
`clang --target=p2 -O2 -c global-boolean.c`:

```c
static int allocated;
int take(unsigned size) {
    int bad = allocated || size != 4;
    allocated = 1;
    return bad;
}
void drop(void) { allocated = 0; }
```

Add a lowering/selection repair and an executable regression before claiming
this form is supported. The [stream-lock audit](runtime-locks/README.md) records
the discovery and the explicit volatile controls used by that unrelated fixture.

## Inactive serial exit helper

`libc/drivers/serial_exit.c` is excluded from the current drivers target. Before
enabling it, update its legacy dependencies, check the temporary stream's open
result, and close the stream before stopping the cog so its hardware lock is
returned. This does not affect the currently built runtime exit path.
