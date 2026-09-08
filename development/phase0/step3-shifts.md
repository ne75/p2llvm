# Step 3 follow-up: zero-count 64-bit shifts

Software checkpoint, 2026-09-07. The three shift helpers are repaired and their
hardware fixtures are built. Hardware execution remains pending; this does not
close step 3 or start step 4. `__negdi2` still needs its return-contract audit.

## Changes to review

| Commit | Review focus |
|---|---|
| `bf5a5b0` | Independent word-at-a-time reference, input/count sweep, ABI probe support and host oracle test |
| `649bbf7` | Left-shift repair and executable fixture |
| `64d5ac1` | Logical right-shift repair and executable fixture |
| `56c8928` | Arithmetic right-shift repair and executable fixture |
| `6869169` | Freeze the original shift dependency of the wide-division timing baseline |
| `eaa1bba` | Prevent the compiler from using tested helpers to prepare or compare reference values |

Each implementation/test commit above changes at most 149 lines, including
additions and deletions.

## Cause and repair

The old helpers used a word shift by `32-count` to bring bits across the word
boundary. At count zero this becomes 32, which the P2 interprets as zero because
SHL/SHR/SAR use `S[4:0]`. The opposite word was therefore ORed into the result.
For example, shifting `0x00000001_00000002` left by zero produced an incorrect
high word of 3 according to the instruction semantics. The instruction reference
in `utils/p2_instructions_revB.csv`, rows 4, 5 and 8, specifies the count masking.

The repaired ASM first performs the same-word shifts, then reuses the count
register with `NEG count,count WZ`. For counts 1..31, its low five bits equal
`32-count`; at zero, Z causes the cross-word shift and OR to be skipped. The
32..63 path subtracts 32 directly from the same count operand.

All three functions now declare paired read/write inputs, a read/write count,
an early-clobber result in R30/R31 and the condition-code clobber. They return a
C value normally, with compiler-generated saves and epilogues. The separate R3
scratch register and missing-return warning suppression are gone. No arithmetic
ASM was replaced by C.

## Generated-code comparison

| Helper | Instructions before / after at O2, Os and Oz | Bytes before / after |
|---|---:|---:|
| `__ashldi3` | 21 / 19 | 84 / 76 |
| `__lshrdi3` | 21 / 19 | 84 / 76 |
| `__ashrdi3` | 22 / 20 | 88 / 80 |

Each executed path has one fewer arithmetic/setup instruction. Optimized saves
remain one SETQ/WRLONG group and one SETQ/RDLONG group; their register save area
shrinks from 16 to 12 bytes (R0..R3 to R0..R2), excluding CALLA's return address.
The runtime archive uses Oz. This is a static code/stack comparison, not a
measured cycle-speed claim.

O0 retains the cost of explicit C/ASM value handling: left/logical right grow
from 39 to 62 instructions, arithmetic right from 40 to 63. The O0 save area
grows from 16 to 24 bytes while its local frame remains 44 bytes. There are no
nested helper calls in any repaired routine at O0/O2/Os/Oz.

## Tests and independent expectations

Each of the three hardware suites checks 136 values at all 64 legal counts:
eight edge patterns, every single set bit, and every single cleared bit. This
gives 8,704 combinations per suite/image. Every combination exercises both a
direct helper call with R0..R29/PTRA snapshots and a separate noinline C shift
expression. Counts 0, 31, 32 and 63 are included. Counts >=64 are outside the
defined C shift domain and are not used as correctness cases.

The expected result is calculated a bit at a time using only unsigned 32-bit
shifts by 1 or 31. The host test compiles that same C reference and compares it
with Python's arbitrary-precision arithmetic for 63,744 value/count/kind checks,
including deterministic random values. A native C compiler named `cc` is used;
set `HOST_CC` if another host compiler is required.

Inspection initially found shift-helper calls introduced while LLVM packed and
unpacked the fixture's C unions. Inputs now use explicit volatile word accesses;
actual outputs use paired ASM word moves. Expected results remain separate
32-bit words. O0/O2/Os integration checks reject helper calls anywhere outside
the intentional C shift expression. This prevents a faulty helper from also
constructing its own expected value. The word-move mechanism has the separate
`inline-register-pairs` fixture in the same hardware checkpoint.

All 25 integration tests and 13 runner/oracle unit tests pass. The complete
ISA/performance build produced 504 images; the nine shift images were rebuilt
after the fixture-independence repair. Both raw build records are preserved,
with the nine later records superseding the corresponding earlier ones. These
are build successes, not hardware passes.

The old wide-division timing helper previously called the runtime's old Oz
`__lshrdi3`. Its dependency is now frozen as `udivmod_previous_shift`; the 84
machine bytes match the original Oz helper exactly. Changes to runtime shifts
therefore do not alter the old timing path. This frozen benchmark assembly is
not a new implementation of a runtime function.

## Current hardware checkpoint

This command supersedes the earlier twelve-image checkpoint. It runs 24 images:
the three shifts, three arithmetic helpers, register-pair fixture and wide-divide
timing, each at O0/O2/Os. The original signed-division hardware evidence and the
earlier software/runtime snapshots remain preserved separately.

```sh
cd ~/Code/p2llvm && python3 -u tests/hardware/run.py --mode hardware --performance --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000 --timeout 300 --case runtime-shift-left --case runtime-shift-right --case runtime-shift-arithmetic --case runtime-signed-division --case runtime-wide-multiply --case runtime-wide-division --case inline-register-pairs --case performance-wide-division
```

Preserve `hardware/results.json`, observations, logs and images before another
hardware invocation. No chip result or shift-helper timing is claimed by this
software checkpoint. The old wide-divider remains incorrect on timing input 4;
its timing is not an equivalent correct-operation comparison.

`step3-shifts-checkpoint.json` records hashes and the review archive under ignored
`build/phase0-checkpoints/step3-shifts-2026-09-07/`. Copy the archive with the review
handoff; Git does not preserve it. Sources, before/after assembly, machine bytes,
verified build records and small per-commit patches are included.
