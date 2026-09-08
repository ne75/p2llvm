# Step 2 hardware report — 7 September 2026

**Step 2 implementation and validation are complete; ready for human review.**
The initial run reported 18/18 PASS. Its O2 memset timing was invalid because
optimization collapsed 64 identical fills into one. Commit `7db2039` fixes the
harness; the corrected three-image hardware rerun passed. The accepted set is
18 suite/optimization combinations: 15 from the initial run plus three corrected
memset benchmarks. All 21 raw run records are preserved, including the invalid
initial timing. No timeouts occurred. Step 3 has not started.

## Correctness

| Hardware suite | O0 | O2 | Os | Evidence |
|---|---|---|---|---|
| runtime-memset | PASS | PASS | PASS | 460 cases per optimization; zero bad bytes or return pointers; LUT readback 0x34 |
| runtime-memory | PASS | PASS | PASS | Existing copy/move/zero-length/return-value observations match |
| runtime-counters | PASS | PASS | PASS | 32-bit and 64-bit counter observations match |
| counter-rollover | PASS | PASS | PASS | High word changes from 0 to 1 across the first low-word wrap |

Memset covers 23 lengths, four byte alignments and five fill values, including
zero length, truncation, negative values, FIFO boundaries and 1024-byte fills.
Every arena byte is checked, including guards. The LUT readback checks completion
before returning to HUB code. Across three optimizations this is 1,380 cases.

| Counter rollover | Before high | After high | Elapsed sysclks |
|---|---:|---:|---:|
| O0 | 0 | 1 | 65,528 |
| O2 | 0 | 1 | 65,616 |
| Os | 0 | 1 | 65,640 |

The elapsed high word is zero in every rollover test. These samples exercise
a nonzero return in R31, beyond the short counter tests near reset.

## Memset cycles — corrected rerun

Each value below is **sysclks per 64-call batch**, including caller loop/call
overhead. Five alternating samples were taken; each minimum equalled its maximum.
Lengths are bytes, destination offset is three. Empty timings are reported
without subtraction. The production comparison retains the original WRFAST/DJNZ
assembly and adds a completion drain so every timed fill has finished writing.

| Opt | Length | Empty | Previous C loop | Original ASM + drain | Updated WRFAST | C / updated |
|---|---:|---:|---:|---:|---:|---:|
| O0 | 8 | 16,088 | 74,448 | 24,776 | 28,376 | 2.62× |
| O0 | 64 | 16,088 | 450,768 | 46,792 | 50,392 | 8.95× |
| O0 | 1024 | 16,088 | 6,901,968 | 415,432 | 419,032 | 16.47× |
| O2 | 8 | 4,603 | 14,328 | 10,741 | 10,746 | 1.33× |
| O2 | 64 | 4,603 | 72,696 | 32,245 | 32,250 | 2.25× |
| O2 | 1024 | 4,603 | 1,071,096 | 400,885 | 400,890 | 2.67× |
| Os | 8 | 4,603 | 13,816 | 10,741 | 11,258 | 1.23× |
| Os | 64 | 4,603 | 72,184 | 32,757 | 32,762 | 2.20× |
| Os | 1024 | 4,603 | 1,070,584 | 401,397 | 401,402 | 2.67× |

- Against the previous C loop, optimized fills are 1.23–2.67× faster in these batches.
- At O2, the updated implementation is five cycles above the completed original
  assembly for the entire 64-call batch, at all three lengths.
- At Os, 8-byte fills cost 517 extra cycles per batch (8.08 per call, 4.81%);
  64/1024-byte fills cost five extra cycles per batch.
- At O0, updated fills cost 3,600 extra cycles per batch (56.25 per call) versus
  the completed original assembly: 14.53%, 7.69% and 0.87% at 8/64/1024 bytes.
  The unoptimized implementation has more bookkeeping. It is still faster than
  the previous C loop. No claim of universally unchanged performance is made.

These are measurements for this firmware layout and board run. Tiny differences
can depend on HUB/FIFO phase; five equal samples do not establish a universal
cycle constant. The runtime remains handwritten ASM with zero-length and write
completion handling. Production runtime builds use Oz; its memset instruction
sequence matches the optimized source bodies checked here, but this is not a
separate whole-application production or Oz-layout benchmark.

## Counter cycles

| Opt | Empty batch | Previous C combine | Direct R30/R31 leaf | Previous / updated |
|---|---:|---:|---:|---:|
| O0 | 9,928 | 24,280 | 9,944 | 2.44× |
| O2 | 4,475 | 12,660 | 4,469 | 2.83× |
| Os | 6,119 | 14,311 | 6,119 | 2.34× |

Units and sampling are the same 64-call batches. All nine linked counter
wrappers were inspected: O2 has 64 unrolled calls; O0/Os retain counted loops.
The updated leaf is GETCT R31 WC, GETCT R30, RETA: three instructions / 12 bytes,
with no shift-helper call. It is linked from the Oz runtime; the body is identical
at O0/O2/Os. Batch speedups are 2.34–2.83×. Empty O2 is six cycles above updated;
subtracting it would give a misleading negative cost. These are not isolated
instruction-latency measurements.

## Benchmark repair and checks

The original O2 updated-memset observations were 156 / 492 / 6,252 cycles.
Linked code and LLVM IR showed one fill without a loop. Those values are retained
as invalid measurements and are not divided by 64 or used in speed comparisons.
An empty compiler memory barrier after every fill prevents merging and applies
equally to all four comparison paths. It emits no P2 instructions.

Commit `7db2039` is 29 added lines and one removed line across three files.
The new integration regression rejects the old single-call assembly and passes
the corrected O2/Os loops. All 17 integration tests pass; corrected firmware
builds and hardware executions pass at O0/O2/Os. The twelve linked memset
wrappers all retain 64 calls. Runtime and compiler code were unchanged by this
benchmark repair. Earlier 368 instruction, 139 LLVM, 12 runner/oracle, 39 host
PASS / 27 host-not-applicable, and 480 firmware build results remain preserved.

## Evidence and limits

Raw protocol records were re-decoded; identities, complements, completion counts
and all expectations match. Compiler/runtime, source, manifest, ELF and binary
hashes were checked before archiving each run. Board model and measured clock
frequency were not supplied. The loader identified ROM G on
`/dev/cu.usbserial-DK0H6QJS`, clock mode `0x012427f8`, RTS reset and 2,000,000 baud.
All timings are raw sysclks; no clock frequency is assumed.

- Initial runtime/test revision: `ed8af6e6e32fe4e72f53a0ac7c34cf11255b9b5c`.
- Corrected benchmark revision: `7db2039365544679c8fba1a1106b505233a06892`.
- Compiler SHA-256: `898c4430bad227c208bc933f726925adb7ec198e370aa80d445bdd1a1f5522bc`.
- Runtime SHA-256: `3ccd7ae3fb6c0b326590d5d3b3c4b97b3766d7e872f9908d654fc3380ace2699`.
- Initial archive: `build/phase0-checkpoints/step2-hardware-2026-09-07/p2-step2-hardware-original.zip`.
- Initial archive SHA-256: `ad114fecc0d38319d1b936a63da511abb3bd7a0746730cfc7bd279ddc8847fef`.
- Rerun archive: `build/phase0-checkpoints/step2-hardware-rerun-2026-09-07/p2-step2-hardware-rerun.zip`.
- Rerun archive SHA-256: `1106f805b28f280c2dcd8d3973ff94dc119b7fbbf4db098f0e15aed83f1f69a4`.

This is the step-2 targeted checkpoint, not a rerun of the complete instruction
suite on the changed runtime. The remaining runtime-helper audit, instruction
coverage, production/application comparison and backend contracts remain in the
agreed later steps. Review the short-fill/O0 overhead before advancing to step 3.
