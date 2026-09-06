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
The short counter fixture does not validate a low-word rollover. A dedicated
rollover run and accurate peripheral/interrupt timing need the actual board.
The COG/lock fixture starts a separate cog, observes contention and release,
and stops the worker; its handshake is covered by the host's hard timeout.

`isa_events.py` checks counter targets and selectable LUT-read events without
GPIO wiring. `isa_fifo.py` executes from LUT RAM because HUB execution owns the
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
