# Step 5: public production-baseline comparisons

The generic C/C++ benchmark checkpoint is ready for human review. Compare the
saved production toolchain/runtime against cleanup using the [full tables](step5-basics.md)
and [reusable testing guide](../../tests/hardware/basics/README.md).

## Validation

Production uses root `bdcefcce7860`, LLVM `72a9bb1ef265`; cleanup runtime uses
`e1e93d6`, LLVM `5ea75453d906`. Both use LLVM 14 and matching isolated SDK sources.
Runtime archives are built at Oz. Kernel/harness builds cover O0/O2/Oz.

All six final benchmark images pass, with 792 checked observations. The select
and float regression follow-up passes another six images and 195 observations.
The instruction matrix passes 368 records, LLVM tests pass 142, and integration
passes 27. The retained public evidence contains only synthetic tests and
runtime regressions; identifying application metadata has been removed.

## Whole-runtime linking

The normal driver retains every runtime LUT section through whole-archive linkage.
The full runtime initially occupied 2,624 / 2,048 bytes at Oz. The 848-byte signed
64-bit-to-double body now executes in HUB behind an 8-byte LUT entry, bringing
retained runtime LUT use to 1,784 bytes. Its isolated latency remains unmeasured.
The whole-runtime budget regression checks this application-style link requirement.

The SELECTCC expansion also now preserves repeated source lifetimes: a compared
operand can remain live as a selected result. MIR and executable selection
fixtures cover this repair. Handwritten runtime assembly is retained.

## Performance interpretation

The tables report median cycles per call across five batches of 64 calls at
160 MHz, including dispatch, loop and timer overhead. The empty case exposes
common measurement costs; samples within one boot are not independent runs.
Stack numbers are compiler-reported fixed frames, not high-water measurements.

At O2, the 1 KiB copy changes from 871.6 to 879.7 cycles/call; completed fill
changes from 6,487.6 to 6,503.7. O0's stack-array workload is 36.7% slower.
Per-access local alignment and conservative branch expansion remain performance
follow-ups. Passing semantics does not establish unchanged timing or flight
acceptance. The [evidence index](step5-checkpoint.json) records the local public
archive checksum; no remote backup of that archive is claimed.
