# Hardware validation boundary

The default test command never flashes or starts hardware. Host MC/IR/link tests
cannot establish peripheral timing, CORDIC scheduling latency, interrupt behavior,
or flight suitability.

The hardware suite must run at O0/O2/Os with the same compiler/runtime revision.
Use a dedicated test device, explicit port/board/clock settings, a bounded runner,
and a result record containing compiler hash, firmware hash, optimization level,
board identity, and individual case results. A missing device is a skipped
hardware run, never a pass.

Minimum acceptance cases: the arithmetic and ABI counterexamples tracked in
`docs/phase0/findings.md`, nested HUB calls and callee-save preservation, runtime
memory/string and float helpers, cog startup/return, locks, interrupts/debugging,
and CORDIC/FIFO-sensitive sequences. The production baseline retains the old
board-specific experiments for reference; they are not automated assertions.
