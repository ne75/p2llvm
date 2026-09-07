# Hardware test observation protocol

This document describes the protocol implemented by [transport.c](transport.c),
[protocol.py](protocol.py), and [run.py](run.py) in this directory. It covers
observations sent by test firmware after startup; the P2 ROM download protocol
used by `loadp2` is separate.

The P2 reports named values. Python on the host compares them with independent
expectations and assigns PASS or FAIL. A successful download, a complete
transcript, or a zero loader exit code alone is not a passing test.

## One run

For each selected suite and optimization level (O0, O2, and Os by default),
`run.py` creates a fresh `uuid.uuid4().hex` run ID and embeds it in a separately
built executable as `P2_TEST_RUN_ID`.

In hardware mode, the runner links the local P2 runtime, converts the ELF to a
binary, and starts `loadp2`. The loader resets the board, clears RAM, downloads
the program, patches clock/serial parameters, and enters terminal mode. Firmware
then performs this sequence:

1. Initialize UART, using RX pin 63 and TX pin 62.
2. Emit `P2TEST` with the embedded run ID.
3. Emit the three transport observations listed below.
4. Call the suite's `test_body()`, which reports observations through `observe()`.
5. Emit `P2END` with the same run ID and the number of observations emitted.
6. Send the loader's binary exit sequence, `FF 00 00`.

The host captures the loader output and validates it after the loader exits.
There are no observation-protocol requests, acknowledgements, or retries during
the run. Every suite/optimization combination starts a new firmware download.

Host mode uses the same text records on standard output, with a native compiler
and runtime. It omits UART initialization and the binary loader-exit sequence.
Build mode stops after linking and records `BUILT_NOT_RUN`; it produces no
observation transcript.

## Record format

Firmware emits ASCII text with LF (`\n`) line endings. Fields use single ASCII
spaces. The decoder uses Python's `splitlines()`, so captured CRLF line endings
also work. Keywords and names are case-sensitive. Do not emit blank lines,
extra spaces, or diagnostic messages inside the observation frame.

```text
P2TEST <run_id>
P2VALUE <name> <value> <complement>
P2END <run_id> <count>
```

| Field | Encoding and meaning |
|---|---|
| `run_id` | The runner generates 32 lowercase hexadecimal characters. The decoder requires an exact match with the supplied expected ID; it does not independently validate UUID syntax. |
| `name` | One or more characters matching `[a-zA-Z0-9_.-]+`. Names must be unique within the frame. There is no escaping. |
| `value` | Exactly eight lowercase hexadecimal digits, without `0x`, representing an unsigned 32-bit word. |
| `complement` | Exactly eight lowercase hexadecimal digits. Must equal `value ^ 0xffffffff`. |
| `count` | Exactly eight lowercase hexadecimal digits. Counts all `P2VALUE` records, including transport checks and both halves of 64-bit observations. It excludes `P2TEST` and `P2END`. |

`observe()` writes the supplied name without validating it. Fixture authors
must use legal, unique names. Observation order does not affect comparison;
the firmware currently emits transport checks first, but the decoder checks
their names and values rather than their positions.

Every firmware starts with these observations. Their expectations are supplied
by `transport_expected` in [cases.json](cases.json):

| Name | Value | Complement |
|---|---|---|
| `transport.zero` | `00000000` | `ffffffff` |
| `transport.ones` | `ffffffff` | `00000000` |
| `transport.alternating` | `a55a5aa5` | `5aa5a55a` |

## Complete example

This illustrative suite observes `quotient(100, 7)`, whose expected value is 14.
The run ID below stands in for the fresh ID embedded by the runner.

```text
P2TEST 8c017ad642124dbfab6e14b7b2d0d392
P2VALUE transport.zero 00000000 ffffffff
P2VALUE transport.ones ffffffff 00000000
P2VALUE transport.alternating a55a5aa5 5aa5a55a
P2VALUE quotient 0000000e fffffff1
P2END 8c017ad642124dbfab6e14b7b2d0d392 00000004
```

In hardware mode, three raw bytes `0xff, 0x00, 0x00` follow the final newline.
They are not the ASCII text `FF 00 00`, and they are not another observation.
`loadp2 -q` consumes them as its exit command with exit status zero. The text
decoder does not use them as its completion marker; it requires `P2END`.

## Reporting values from a fixture

Implement `void test_body(void)` and include `observe.h`:

```c
#include "observe.h"
extern unsigned quotient(unsigned, unsigned);
extern unsigned long long wide_value(void);

void test_body(void) {
    observe("quotient", quotient(100, 7));
    OBSERVE64("wide", wide_value());
}
```

`OBSERVE64` evaluates its value once, then emits `wide.lo` followed by `wide.hi`.
Each is an ordinary 32-bit observation with its own complement and count entry.
The host compares both words separately. Reconstruct the full bit pattern as
`(hi << 32) | lo` when inspecting results.

There are no wire-level type tags. Signed 32-bit results are represented by
their unsigned bit pattern: -1 is `ffffffff`. Floating-point fixtures must
explicitly choose what to report, such as a bit pattern or a numeric property;
the protocol does not serialize floating-point decimal text.

Expected values belong in the suite's `expected` object in `cases.json`, or in
the independently written Python model for a generated ISA suite. The runner
merges these with `transport_expected`. Keep observation names distinct from the
reserved `transport.*` checks; a duplicate manifest key would override the
transport expectation during that merge.

```json
{
  "quotient": "0xe",
  "wide.lo": "0x89abcdef",
  "wide.hi": "0x12345678",
  "elapsed": {"min": "0x40", "max": "0x989680"}
}
```

This is an expectation-format example; a suite must emit exactly the names in
its own expectations, plus the transport checks. Values are strings parsed
with `int(text, 0)`, so decimal and `0x` hexadecimal forms work. A range must
contain exactly `min` and `max`, with inclusive bounds satisfying
`0 <= min <= max <= 0xffffffff`. Signed comparisons and 64-bit ranges are not
implicit: encode the intended assertions explicitly in the fixture.

## Decoder and verdict rules

`protocol.compare(output, run_id, expected)` returns an observed-value mapping
and a list of mismatches, or raises `ValueError` for malformed framing,
observations, or expectation specifications.

1. Decode captured bytes as ASCII, replacing invalid bytes, and split into lines.
2. Require exactly one line equal to `P2TEST <expected_run_id>` in the entire
   capture. This rejects a stale-only capture and duplicate matching starts.
3. Read subsequent lines until the first correctly formatted `P2END` with that
   same ID. Every preceding line must be a valid `P2VALUE` record.
4. Reject duplicate names, invalid complements, missing completion, or a final
   count different from the number of observations received.
5. Compare the complete set of observed names with the expected set. Missing
   and unexpected names are mismatches, as are wrong values or values outside
   an expected range.

Loader messages before the matching start line are ignored. Text after the
first valid end line is also ignored, except that another matching start line
anywhere in the capture fails step 2. The decoder does not require EOF directly
after `P2END`, reject all trailing records, or validate frames for other run IDs
outside the selected frame. It parses one expected frame, not a stream of runs.
Invalid bytes or debug text inside that frame fail record parsing.

The complement check detects inconsistent value/complement pairs; it is not a
checksum over the whole transcript or cryptographic authentication. The run ID
identifies the expected invocation, not a hash attested by the board. Artifact
hashes are recorded separately by the host.

The runner assigns PASS only after the execution subprocess completes
successfully and the decoder returns no mismatches. A complete `P2END` followed
by a loader timeout is still a failure. A valid frame containing the wrong
answer is a semantic-comparison failure, even if the loader exits zero.

## Loader settings, deadlines, and evidence

[loader.py](loader.py) supplies `-v -q -t -ZERO -PATCH`, the selected DTR/RTS reset,
application baud (`--baud`, default 115200), and download baud (`--loader-baud`,
default 2000000). Port, clock, and host serial FIFO overrides are optional.
If port or clock settings are omitted, the loader handles detection/defaults.
Board identity is host-side metadata, not a wire field. See [README.md](README.md) for
board setup and commands.

The runner holds the loader's stdin pipe open so terminal mode does not exit
early on stdin EOF. `--timeout` defaults to 30 seconds for the loader invocation,
including download, startup, execution, and output collection. Compile/link
subprocesses have their own 120-second default deadlines. There is no per-value
acknowledgement or progress-based deadline extension.

Results live under `<build-dir>/p2-hardware/hardware/`:

| File | Contents |
|---|---|
| `results.json` | Per-suite status, optimization, run ID, observations, mismatches/errors, failure stage, timing, artifact hashes, and available loader/device metadata. |
| `<suite>-<optimization>/commands.log` | Commands and combined stdout/stderr, including partial output on command failures and timeouts. |
| `<suite>-<optimization>/observations.log` | Captured loader output; partial output is retained for loader command failures/timeouts. |
| `<suite>-<optimization>/test.elf`, `test.bin` | Linked executable and binary supplied to the loader. The binary hash identifies the file before the loader patches clock/serial parameters. |

Failure stages distinguish compilation, linking, binary conversion, hardware
execution, observation-protocol validation, and semantic comparison. A serial
configuration failure or timeout is not evidence of a wrong computed result.
The runner exits nonzero if any selected case fails. Each invocation replaces
its mode's aggregate `results.json`; selected case directories are reused too.
Archive results separately when preserving a particular run.

## Checking the protocol implementation

Run the existing protocol tests from the repository root:

```sh
python3 -B tests/hardware/test_protocol.py
```

They cover correct values, mismatches, missing/extra names, inclusive ranges,
stale IDs, corrupt complements, missing completion, bad counts, and duplicate
observations. `tests/driver/protocol.test` also runs them through lit. When
changing the format, update the firmware emitter, Python decoder, tests, and
this document together. There is currently no protocol-version field or
version negotiation.
