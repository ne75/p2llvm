# Hardware semantic tests

Every executable regression must have a hardware observation fixture. Compiler
diagnostics and malformed-input tests are host-only by construction; their valid
counterparts still need an executable fixture. An assembly/encoding pass does
not satisfy the hardware requirement.

`cases.json` contains independently specified expected values. The same source
files used by the C/C++ integration tests are linked with small observation
drivers and executed at O0/O2/Os. Sources and drivers are separate translation
units and LTO is disabled, so the test input cannot be folded into the function
being tested. The round-trip fixture executes the reassembled compiler output.

The firmware emits named raw values and their complements. The host checks
every value, the complete set of names, a per-build run identity, and the final
observation count. The target does not emit a PASS verdict. Missing, duplicate,
corrupt or unexpected observations fail, and execution has a hard timeout.
Transport self-checks precede each suite. `test_protocol.py` exercises rejection
of corrupt, incomplete and stale streams.

See the [hardware test observation protocol](PROTOCOL.md) for the exact record
format, a complete transcript, value encoding, validation rules, and loader
completion and failure handling.

```sh
# Validate the portable fixtures and handwritten expected values on the host.
python3 tests/hardware/run.py --mode host

# Build the runtime from the same local compiler; no installed SDK is required.
cmake -S libp2 -B build/phase0-libp2 -G Ninja \
  -Dllvm="$PWD/build/phase0-llvm/bin"
cmake --build build/phase0-libp2 --target p2

# Build every registered P2 firmware, without opening a serial device.
python3 tests/hardware/run.py --mode build

# Execute using the actual board's port and clock configuration.
python3 tests/hardware/run.py --mode hardware \
  --loader /absolute/path/to/loadp2 --port /dev/cu.YOUR_DEVICE \
  --board YOUR_BOARD_ID --clock-hz YOUR_HZ --clock-mode YOUR_MODE
```

`--case ID` and `--optimization O0|O2|Os` select a subset. Result JSON, complete
command logs, firmware, ELF images and raw observations are under
`build/phase0-llvm/p2-hardware/MODE/`. Records include source, manifest, compiler,
runtime, ELF and firmware hashes, optimization level, board/port/clock settings
and individual mismatches. `BUILT_NOT_RUN` is distinct from PASS.

For loadp2's automatic USB serial selection and clock defaults, omit `--port`,
`--clock-hz`, and `--clock-mode`. This board setup uses RTS reset, 2 Mbaud, and a
10,000-byte host serial FIFO:

```sh
python3 -u tests/hardware/run.py --mode hardware --isa --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000
```

The runner adds `-v` to retain ROM-version/port detection, `-PATCH` so the runtime
receives clock/baud parameters, and `-q` for unattended terminal exit. It holds
stdin open until completion because loadp2 otherwise exits on stdin EOF. Loader
errors and timeouts retain partial output and identify the failed stage.
Unspecified board/clock metadata remain unspecified; detected values are
recorded from loader output rather than guessed.

Serial configuration can be denied by the execution environment even when the
device can be opened. A `tcsetattr`/baud-setting failure is a loader failure,
not a P2 semantic failure. The full suite should only follow a working initial
load; do not count unexecuted cases as passing.

The hardware transport currently uses the existing UART runtime on pins 63/62.
The loader uses RAM download with `-ZERO -PATCH -q -t`; no persistent flash
programming is requested. A timeout includes startup and transport failures,
not just a failed test body. The runner never retries a failed test into a pass.

Instruction-level coverage must pair the TableGen inventory with independent
ISA semantic fixtures. The structural matrix alone is not hardware coverage.
Peripheral/event/debug instructions need explicit setup, observable effects,
cleanup, and board fixture requirements. Until those fixtures are present and
run, the instruction hardware coverage remains incomplete.

## Validation boundary

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

## Instruction semantics and coverage gate

`python3 tests/hardware/run.py --mode build --isa` includes generated instruction
scenarios. Use the same `--isa` flag in hardware mode. TableGen supplies the
inventory and operand syntax; `isa_model.py` and `isa_scenarios.py` supply
independent expected register values, flags, memory results, and branch outcomes.
The generated assembly, drivers, expectations, and oracle hashes are reviewable
under `build/phase0-llvm/p2-isa/`. Memory/LUT scratch contents are restored.

`python3 tests/hardware/coverage.py` writes a per-record CSV and missing-record
JSON under the normal test-results directory. `--require-complete` fails if any
instruction lacks an executable fixture. This checks fixture readiness; only a
matching hardware-mode result establishes execution. Coverage remains incomplete.

Counter expectations use bounded raw elapsed counts; the host checks the range.
The short counter fixture does not validate a low-word rollover; the optional
`counter-rollover` fixture below does. Peripheral/interrupt timing still needs
additional board fixtures.
The COG/lock fixture starts a separate cog, observes contention and release,
and stops the worker; its handshake is covered by the host's hard timeout.

`isa_events.py` checks counter targets and selectable LUT-read events without
GPIO wiring. Counter tests follow the Rev B/C v35 reference, printed pages
42-46: POLL/WAIT clear an event unless its sensor sets it again. A past CT target
keeps asserting the event; ADDCT with a future target clears and rearms it.
The fixtures check before the deadline, after it, a repeated poll with the
target still in the past, and after rearming 65,536 ticks into the future.

| Counter fixture | Low return word | High return word |
|---|---|---|
| POLLCT1/2/3 | C at each phase, bits 0-3: `0b0110` | Z at each phase, bits 0-3: `0b0110` |
| ADDCT1/2/3 and WAITCT1/2/3 | Elapsed ticks: `0x10000..0x10400` | C,Z pairs at each phase, bits 0-7: `0x3c` |

The first phase occupies the low bit/pair. POLLCT uses WAITX to let the deadline
pass independently of WAITCT. ADDCT/WAITCT retain their elapsed-time check and
test both flags after WAITCT. Both paths rearm before the final poll so an event
that never clears fails as well as one that never asserts. These expected values
come from the documented event conditions; a successful build is still
`BUILT_NOT_RUN` until the updated fixtures execute on the chip.

The ALTS/ALTD fixtures retain augmented immediate and register operands. The
Rev B/C v35 silicon erratum (printed page 4) says ALTx with immediate S uses
AUGS without cancelling it. After capturing both results, immediate fixtures
execute `mov pa, #0` to consume any pending augmentation in scratch PA before
returning to C. The expected register/index results are unchanged.

`isa_fifo.py` executes from LUT RAM because HUB execution owns the
FIFO; it waits for pending writes before readback and return. QROTATE/QVECTOR
fixtures use coarse quadrant/scale windows (ideal result ±4096 units), not a
silicon accuracy specification. Exact edge/rounding and saturation cases remain
additional work. XORO32's model is based on the chip designer's published
[Rev B/C implementation](https://forums.parallax.com/discussion/comment/1448460/)
and checks the published seed-1 output before generating fixtures.

The coverage gate currently tracks instruction-record readiness, not exhaustive
operand, flag, predicate, address-mode, or timing coverage. A ready record can
still need boundary cases. Hardware runs must review actual mismatches before
changing an expectation; a compiler-produced value is not an independent oracle.

## Runtime performance review (step 2)

`runtime-memset` compiles the actual implementation at O0/O2/Os. It checks 460
combinations of length (0 through 1024, including FIFO/block boundaries), all
four byte alignments, and five fill values including truncation and negative
values. Every byte and both guards are checked, along with the return pointer.
A separate readback stays in LUT until after reading the last filled byte.

`--performance` adds optional fixtures from `performance.json`. Timing fixtures
measure 64 calls per batch with independent GETCT reads, alternate implementations
for five samples, and report raw minimum/maximum sysclks. UART output is outside
the timed region. Empty-call timings include loop/call overhead and are reported
separately; no overhead is silently subtracted. Timing ranges only reject empty
or implausibly long observations: PASS does not establish a speed improvement.

For `performance-memset`, size indices 0/1/2 mean 8/64/1024 bytes at offset 3.
Implementation indices 0/1/2/3 mean empty / previous volatile byte loop / original
WRFAST-DJNZ loop with an added drain / updated memset. Nonzero lengths avoid the
original loop's zero-count bug. All fill timings include completed writes.
`performance-counter` indices 0/1/2 mean empty / previous C combine / updated
_cnt64. Memset and comparison sources use the selected optimization; _cnt64 is
linked from the -Oz runtime, with its identical three-instruction body verified
at O0/O2/Os. The sysclk rate is not inferred from the cycle counts.

`counter-rollover` waits independently for the first 32-bit CT wrap after reset,
then checks _cnt64 before and after it, including a nonzero high half. The wait
is approximately 2^32/sysclk seconds; allow startup time as well. It is excluded
from the ordinary suite because it can exceed the default 30-second timeout.

```sh
python3 -u tests/hardware/run.py --mode hardware --performance --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000 --timeout 300 --case runtime-memset --case runtime-memory --case runtime-counters --case performance-memset --case performance-counter --case counter-rollover
```

The command runs 18 images (six suites at O0/O2/Os) and replaces hardware-mode
results as usual. Preserve that JSON and the logs before starting another run.
