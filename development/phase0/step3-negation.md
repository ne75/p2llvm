# Step 3: final arithmetic repair and combined hardware checkpoint

Software checkpoint, 2026-09-12. `__negdi2` now declares its ASM inputs and result,
completing the remaining return-contract repair identified during step 3. The
later [hardware report](step3-hardware.md) verifies all 27 images and records
division timing tradeoffs. Step 3 is ready for human review; the pending status
below describes this software checkpoint. Step 4 has not started. This checkpoint supersedes the hardware commands in the earlier
[arithmetic](step3.md) and [shift](step3-shifts.md) checkpoints.

## Change to review

Commit `f570842` repairs wide negation and adds its regression/ABI fixture: six
files, 158 additions and nine deletions. The original function wrote R30/R31
without declaring an ASM result or returning a C value. Its optimized LLVM IR
therefore returned `undef`, despite the expected machine-register contents.

The repaired normal C function declares a paired early-clobber output bound to
R30/R31, a read-only paired input and a condition-code clobber. It returns the
declared result. LLVM owns the function boundary; all four handwritten arithmetic
instructions are retained. No missing-return warning suppressions remain in
`libp2/lib/builtins`.

The regression was run against the old source first and failed on the undefined
IR return. It now checks the declared IR result, machine verification, optimized
assembly and independence of the executable test's expected values.

## Generated code

| Optimization | Instructions before / after | Machine bytes identical? | Nested calls |
|---|---:|---|---:|
| O0 | 23 / 35 | No | 0 |
| O2 | 5 / 5 | Yes | 0 |
| Os | 5 / 5 | Yes | 0 |
| Oz (runtime archive) | 5 / 5 | Yes | 0 |

The complete optimized function is 20 bytes:

```asm
not  r30, r0
not  r31, r1
add  r30, #1 wc
addx r31, #0
reta
```

Inputs R0/R1 are not modified, and the function needs no register saves or stack
frame at these optimized levels. O0 has additional value-handling overhead.
This comparison uses the same cleanup compiler for both sources; it is not the
broader production-toolchain timing comparison planned for step 5.

## Tests

`runtime-wide-negation` tests 32 literal input/result pairs at O0/O2/Os. Inputs
include zero, low/high-word carry boundaries, signed limits, distinct halves and
deterministic random patterns. A Python exact-integer test verifies every literal
expectation as `(-input) modulo 2^64`.

Each direct helper call checks its result and snapshots R0..R29/PTRA for ABI
preservation. Separate noinline functions exercise unsigned and signed C negation.
Signed C excludes `INT64_MIN`, where negation would overflow; its wrapping bit
pattern is tested through the helper directly and defined unsigned C. This gives
32 direct, 32 unsigned-C and 31 signed-C value checks per image. Fixture assembly
checks ensure no arithmetic helper constructs its own expected values.

The runtime was rebuilt after the repair. All 26 integration tests and 14
runner/oracle unit tests pass. All 507 ISA/performance firmware images built
successfully (169 suites at O0/O2/Os). Their compiler, runtime, source and ELF hashes
are verified in `step3-negation-checkpoint.json`; builds are not chip passes.

## Current hardware checkpoint

Run these nine suites at O0/O2/Os: 27 images, including the new negation fixture,
three shifts, signed division, wide multiplication/division, register pairs and
wide-division timing. The earlier three signed-division passes used superseded
source and do not validate this checkpoint.

```sh
cd ~/Code/p2llvm && python3 -u tests/hardware/run.py --mode hardware --performance --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000 --timeout 300 --case runtime-wide-negation --case runtime-shift-left --case runtime-shift-right --case runtime-shift-arithmetic --case runtime-signed-division --case runtime-wide-multiply --case runtime-wide-division --case inline-register-pairs --case performance-wide-division
```

After execution, preserve the aggregate, raw observations, logs, sources and
firmware before another hardware invocation. Recheck every expected observation
and input/binary hash, then report per-optimization results and wide-division
cycle measurements. A benchmark PASS checks timing sanity; review the measured
differences separately. The old divider is incorrect on timing input 4, so that
input cannot establish an equivalent correct-operation speed comparison.

Step 3 can be presented for acceptance after those semantic and timing results
are reviewed. Do not advance to step 4 on software results alone. No automatic
loader retry or assumption that new timeouts are USB failures has been added.

The evidence index records the archive under ignored
`build/phase0-checkpoints/step3-negation-2026-09-12/`. It contains source snapshots,
the implementation patch, before/after IR, assembly and bytes, software logs,
verified firmware build records and the rebuilt runtime. Copy the archive with
the review handoff; Git does not preserve generated evidence.
