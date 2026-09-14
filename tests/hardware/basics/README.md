# Basic compiler/runtime hardware comparison

These portable C/C++ kernels cover integer and single-precision arithmetic,
compared-value selection, aligned/unaligned copies, completed memory fills,
aggregate returns, virtual dispatch, and a stack-resident array. Each kernel
executes 64 inputs. A native build supplies result hashes and timed-loop sums;
all memory bytes and guards are checked outside the timed region. The P2 reports
observations through the existing checked serial protocol. A hardware PASS
requires matching semantics, complete transport, and positive bounded timings.

The driver times five batches of 64 calls with GETCT. Timing includes dispatch,
loop overhead, and timer reads. Inspect the empty case; do not report the raw
number as one instruction's latency. Runtime archives are built separately, so
record their optimization and source revisions. Fixed stack frames from `.su`
files exclude callees, interrupts, and handwritten stack operations.

Use two isolated SDKs, each containing matching headers, linker scripts, tools,
and freshly built runtime libraries. The root compiler driver uses whole-archive
runtime linkage; this benchmark deliberately exercises that application path.
Do not overwrite your installed production SDK. Run the same command once for
each profile, changing its paths and choosing a new result directory:

```sh
python3 tests/hardware/basics/run.py \
  --bin /path/to/profile/bin \
  --libc /path/to/profile/build-libc \
  --libp2 /path/to/profile/build-libp2/lib \
  --out /path/to/new-results --hardware \
  --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000
```

The loader selects the serial port automatically; `--port` pins a specific board.
Use `--no-verifier` only for the saved production compiler, whose old select kill
metadata does not satisfy today's verifier checks. Omit `--hardware` for host
oracle generation and P2 compile/link checks. Both modes generate O0/O2/Oz images.
The default requested clock is 160 MHz; loadp2 derives its mode for its usual
20 MHz crystal configuration. Configure clock settings for a different board.

```sh
python3 tests/hardware/basics/report.py /path/to/production-results \
  /path/to/cleanup-results > comparison.md
```

Review `comparison.md`, both `results.json` files, and the per-optimization
`commands.log`, `observations.log`, ELF, binary, map and `.su` files. Failed runs
remain failed; there is no automatic retry. A rerun requires a fresh output
directory. Repeated timing samples are within one boot, not independent reboots.

Public benchmark results and SDK provenance belong in the
[development checkpoint](../../../development/phase0/step5.md). Private application
checks use temporary directories outside this repository. These kernels do not
validate peripheral wiring or whole-application peak stack usage.
