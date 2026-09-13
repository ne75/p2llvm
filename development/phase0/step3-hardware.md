# Step 3 hardware checkpoint

Verified September 13, 2026 (America/Los_Angeles). **All 27 images passed in one
run: 24 semantic/ABI images and three wide-division timing images.** No timeouts,
retries or semantic mismatches occurred. Step 3 is ready for human review;
step 4 has not started. The performance tradeoffs below remain explicit review
items, rather than being hidden by the timing fixture's PASS verdict.

Tested root checkout: `153516d` (implementation `f570842`); LLVM source:
`faebaf0abaaaf1aea95cdcc3249bf212f2e4ca60`. All compiler/runtime hashes match the
preceding software checkpoint. The development compiler remains LLVM 14; the
runtime archive is built at Oz, while each tested helper source is independently
compiled into its O0/O2/Os image.

## Semantic and ABI results

| Suite | O0 | O2 | Os | Coverage per image |
|---|---|---|---|---|
| Wide negation | PASS | PASS | PASS | 32 helper inputs, 32 unsigned-C and 31 signed-C checks |
| Left shift | PASS | PASS | PASS | 136 values × 64 counts; direct helper and C expression |
| Logical right shift | PASS | PASS | PASS | 136 values × 64 counts; direct helper and C expression |
| Arithmetic right shift | PASS | PASS | PASS | 136 values × 64 counts; direct helper and C expression |
| Wide multiplication | PASS | PASS | PASS | 41 exact input pairs and C expressions |
| Wide unsigned division | PASS | PASS | PASS | 90 exact pairs; quotient, remainder, null pointer, guards, C expressions |
| Register pairs | PASS | PASS | PASS | Constants, carry, wrap, high words and equal-input early clobber |
| Signed division | PASS | PASS | PASS | 24 exact input pairs |
| Wide-division timing | PASS | PASS | PASS | Five inputs × three implementations × five samples of 64 calls |

All seven arithmetic-helper suites report `abi.bad_registers = 0`, covering
R0..R29 and PTRA around direct calls. Every arithmetic mismatch counter is zero.
The shift suites cover 78,336 value/count combinations across the nine images,
each through both the helper and C expression. Signed C negation excludes
`INT64_MIN`; its wrapping bits are checked directly and through unsigned C.

This validates the current compiler-managed helpers, superseding the earlier
three signed-division passes that used a naked implementation. It is targeted
step-3 validation, not a rerun of all 507 available firmware images.

## Measured division cost

GETCT observations count sysclks. Each measurement includes 64 calls plus the
wrapper/loop overhead; the empty-call batch is reported separately and is not
subtracted. Every recorded minimum equaled its maximum across five samples.
The delta below is `(updated batch - previous batch) / 64`; it is the observed
difference per call in this harness, not an isolated instruction latency.

| Input | Purpose | O0 delta/call | O2 delta/call | Os delta/call |
|---|---|---:|---:|---:|
| 0: 123456789 / 7 | Small dividend/divisor | 0 | -40 | -40 |
| 1: UINT64_MAX / 3 | Two QDIV path | +56 | +16 | +16 |
| 2: (3 × 2^48) / 2^48 | Wide divisor, exact quotient | +48 | +32 | +32 |
| 3: 2^63 / 2^63 | Normalization count 32 | +40 | +16 | +16 |
| 4: UINT64_MAX / (2^32 + 3) | Estimate correction and product overflow | +80 | +40 | +32 |

At O2/Os, input 0 improves by about 13.1% of the whole batch. Inputs 1–3 cost
about 4.0–7.7% more. O0 is unchanged on input 0 and costs about 6.2–10.5% more
on inputs 1–3. These are real measured tradeoffs for the repaired implementation.
The old divider is incorrect on input 4, so its timing cannot establish an
equivalent correct-operation speed comparison.

The comparison uses the frozen previous helper, including its original Oz shift
dependency, with the current cleanup compiler. It is not a complete production
toolchain comparison. The updated optimized wide divider is larger (344 versus
204 bytes) but removes two nested shift-helper calls. Optimized signed division,
multiplication and negation retain their previous machine bytes; the repaired
shifts are smaller. Those static comparisons are recorded in the earlier
[arithmetic](step3.md), [shift](step3-shifts.md) and [negation](step3-negation.md)
checkpoints. Broader stack/code-size and application timing measurements belong
to step 5; this run does not establish production or flight performance acceptance.

### Raw 64-call batch sysclks

Min = max for every row. Empty batches remain visible so the loop/call cost is
not mistaken for useful division work.

| Optimization | Input | Empty | Previous | Updated |
|---|---:|---:|---:|---:|
| O0 | 0 | 21192 | 34008 | 34008 |
| O0 | 1 | 21704 | 34008 | 37592 |
| O0 | 2 | 21192 | 42200 | 45272 |
| O0 | 3 | 21192 | 41176 | 43736 |
| O0 | 4 | 21192 | 41176 | 46296 |
| O2 | 0 | 5658 | 19482 | 16922 |
| O2 | 1 | 5658 | 19482 | 20506 |
| O2 | 2 | 5666 | 26658 | 28706 |
| O2 | 3 | 5658 | 25626 | 26650 |
| O2 | 4 | 5658 | 26650 | 29210 |
| Os | 0 | 5657 | 19481 | 16921 |
| Os | 1 | 5657 | 19481 | 20505 |
| Os | 2 | 5657 | 26649 | 28697 |
| Os | 3 | 5657 | 25625 | 26649 |
| Os | 4 | 5657 | 26649 | 28697 |

## Evidence and review boundary

All 372 observations were decoded again from preserved raw logs and compared
with the exact merged manifest. Run IDs and completion records, source hashes,
ELF/firmware hashes, compiler/runtime/loader hashes and loader identity were
verified. The actual board model and measured clock frequency were not supplied.
The loader identified P2 ROM G on `/dev/cu.usbserial-DK0H6QJS`, clock mode
`0x012427f8`, with automatic port selection, RTS, 2 Mbaud and FIFO 10000.

`step3-hardware-checkpoint.json` records hashes and the local archive under
`build/phase0-checkpoints/step3-hardware-2026-09-13/`. The archive contains raw
results, logs, objects, firmware, source snapshots, expectations, runtime, derived
timings and the prior software evidence. Its CRCs and file hashes were checked.
Git preserves the index and report, not the ignored evidence archive; copy that
archive with the review handoff.

The step-3 implementation, software checks, targeted hardware semantics and
division timing analysis are complete. Review the measured overheads above before
accepting the step and authorizing step 4. Instruction coverage, production
comparisons, ABI/driver cleanup and multi-function COG residency retain their
previously agreed positions in the work list.
