# Step 3: handwritten arithmetic with compiler-managed function boundaries

Initial software checkpoint, 2026-09-07 (America/Los_Angeles). The subsequent
[shift-helper follow-up](step3-shifts.md) records the zero-count fixes, newer
software results and the current hardware command. Step 3 remains open for
hardware correctness, wide-division timing and the negation-helper contract.
Step 4 has not started.

The three named helpers now use ordinary C function definitions with extended
ASM operands and explicit returns. LLVM generates their register saves, stack
argument access and epilogues. The arithmetic remains handwritten P2 assembly.
There are no hand-coded prologues or missing-return suppressions in these helpers.

## Reviewable changes

| Repository / commit | Review focus |
|---|---|
| root `47b68b8` | First signed-division repair, 24 exact cases and ABI snapshot probe; its naked implementation is superseded below |
| LLVM `faebaf0abaaa` | i64 register-pair constraints, low/high operand printing, named pair registers and negative diagnostics |
| root `7812cef` | Compiler gitlink, executable pair/carry fixture and reusable inline-assembly reference |
| root `6af1d1b` | Replace naked signed division with compiler-managed saves and explicit R31 result |
| root `a11b709` | Original wide-multiply ASM with paired operands, explicit R30/R31 result and 41-vector fixture |
| root `e3039c2` | Exact wide unsigned quotient/remainder repair and 90-vector fixture |
| root `0ab3cff` | Optional wide-division timing, frozen old helper and a regression protecting the 64-call loop |

The largest change above is 324 added/deleted lines, including its tests. Review
the LLVM commit separately from the root gitlink, using `git -C llvm-project show
faebaf0abaaa`. All work remains on `phase0_cleanup`; production was not installed
over, changed or pushed by this work.

## Why register-pair support was needed

The existing P2 inline-ASM implementation supported scalar `r` operands but did
not properly expose 64-bit register pairs. Splitting a result into two C values
and joining them with shifts introduced calls to shift helpers. Binding an i64
value to the single register `r30` could silently discard its high half.

The backend now accepts i64 `r` operands and `%L`/`%H` word modifiers. An explicit
`r30_r31` pair binds a 64-bit output to the ABI return registers. Read/write and
early-clobber operands describe destructive input use; scratch registers and
flags are declared. Wide division declares its memory writes as well. See the
[inline-assembly reference](../../docs/P2%20Inline%20Assembly.md) for syntax.

The paired value is returned normally in C. This keeps the compiler responsible
for the function boundary without adding word-combine helper calls. Invalid
scalar/pair bindings and word modifiers on scalar operands receive diagnostics.
The executable pair fixture also checks a carry, full wrap, high-word data and
equal-valued operands to exercise the early-clobber constraint.

## Arithmetic changes and generated code

`__divsi3` retains its sign/magnitude QDIV algorithm. `__muldi3` retains the sign
handling and three QMUL operations. The explicit outputs make their C/ASM
contracts valid; their optimized machine code is unchanged from the pre-step-3
source compiled with the same cleanup compiler.

| Helper | Before / after instructions at O2, Os, Oz | Before / after bytes | Calls in updated helper |
|---|---:|---:|---:|
| `__divsi3` | 13 / 13 | 52 / 52 | 0 |
| `__muldi3` | 38 / 38 | 152 / 152 | 0 |
| `__udivmoddi4` | 51 / 86 | 204 / 344 | 0 |

Signed division and multiplication are byte-for-byte identical at all three
optimized levels, including compiler-generated saves and returns. This is a
same-compiler source comparison, not the broader production comparison in step 5.
At O0 their sizes grow from 28 to 49 and 57 to 84 instructions respectively;
wide division grows from 80 to 133. Debug builds pay for compiler-managed locals.
The runtime archive itself uses Oz. Before/after assembly, extracted machine
bytes and comparison JSON are preserved with the review evidence.

Wide division previously reduced precision and returned an approximate quotient
and a remainder of the shifted values. It also left the remainder untouched
when dividing by one. The repaired handwritten ASM:

- Retains exact hardware QDIV paths for a 32-bit divisor, using a second divide
  when the quotient needs a high word; division by one writes a zero remainder.
- Handles a dividend smaller than a wide divisor directly.
- Normalizes wide operands without calling `__lshrdi3`, treating shift count 32
  explicitly because P2 word shifts use the low five count bits.
- Calculates a quotient estimate, forms its full 96-bit product with the original
  divisor using two QMULs, and corrects an overshoot by one.
- Stores the complete 64-bit remainder only when its pointer is non-null.

The bit above bit 63 in the product must be retained: vectors 62 and 63 exercise
overflowing products that would otherwise look smaller than the dividend.
Division by zero preserves the old helper extension (maximum quotient and an
untouched remainder), but it is outside the C arithmetic test domain. Signed
division also excludes INT_MIN / -1.

### Estimate bound

For nonzero wide divisor `b`, let `s = bit_length(b) - 32` (1 through 32),
`B = floor(b / 2^s) >= 2^31`, and `t = b - 2^s B`, where `0 <= t < 2^s`.
The normalized divide gives `qhat = floor(a / (b-t))`. It is at least
`q = floor(a/b)`. For a 64-bit dividend, the difference between the two real
quotients is `a*t / (b*(b-t)) < (2^s-1) / 2^(2s-2) <= 1`, including s=1
because the preceding inequality is strict. Thus `qhat` is `q` or `q+1`.

Consequently one product comparison and at most one decrement suffice. If
`qhat*b > a`, subtract one from qhat and add b to the subtraction result.
The product is less than `2^65`; keeping the full QMUL product handles its high
carry, and the resulting remainder satisfies `0 <= remainder < b`.

The fixed vectors include every normalization shift, both product-overflow
examples and exact quotient/remainder boundaries. A separate host integer check
compared 200,000 deterministic wide pairs against Python `divmod`; all matched
after correction (2,089 required correction). This verifies the arithmetic model,
not execution of the emitted P2 instructions.

## Validation completed

| Check | Result |
|---|---|
| Instruction inventory / assembly matrix | 368 passed, 0 failed |
| LLVM P2 tests | 141 passed |
| Repository integration tests | 22 passed |
| Runner and oracle unit tests | 12 passed |
| Host semantic runs | 42 passed; 36 P2-only runs not applicable |
| Firmware build with ISA and optional performance fixtures | 495 built, not hardware passes |
| Final wide-division timing fixture rebuild | 3 built, not run |

`step3-software-checkpoint.json` records the compiler/runtime hashes and the
preserved archive checksum. The archive contains raw results, verified build
images, fixture sources, per-commit patches and before/after assembly. It lives
under ignored `build/phase0-checkpoints/`; copy it with the review handoff because
Git alone does not preserve it. A separate local copy is in the Codex outputs.

The final three-image build follows whitespace cleanup in the frozen baseline;
its source hashes supersede those three entries in the earlier 495-build record.
Both raw records are retained. Linked O0/O2/Os timing wrappers were inspected;
the optimized loops retain all 64 helper calls, and integration checks protect
their loop structure. GETCT reports raw batch cycles, including common overhead.
The old helper is incorrect on timing input 4, which is explicitly labelled in
the [hardware guide](../../tests/hardware/README.md#runtime-performance-tests).

The compiler is the LLVM 14 fork, Release with assertions, built using Homebrew
Clang and the existing P2 experimental-target configuration. Rebuilt targets:
`clang`, `llc`, `split-file`, and the separate runtime `p2` target. The machine
verifier is enabled in the fixtures; `%clang` expands to the development binary
with `--target=p2`. Runtime tests compile actual helper sources at O0/O2/Os,
rather than only testing the Oz implementation already in the archive.

## Hardware checkpoint still pending

The first signed-division checkpoint passed at O0/O2/Os: 24 exact inputs per
image, with no register or stack corruption observed. Its original source was
the superseded naked implementation. It is preserved separately under
`build/phase0-checkpoints/step3-signed-division-2026-09-07/` and is not counted as
hardware validation of the current compiler-managed implementation.

These four suites form 12 images; use the combined current command in the
[shift-helper follow-up](step3-shifts.md#current-hardware-checkpoint) to include
them, the three shift suites and wide-division timing in one preserved run.
The three arithmetic suites use 24, 41 and 90 exact vectors
respectively. Each helper call snapshots R0..R29 and PTRA before and after the
call; division also checks full/null remainders and memory guards. Multiplication
and wide division are also exercised through separate noinline C expressions.
The ABI test probe deliberately uses naked ASM to take snapshots before C code
can reload or repair a register. That test instrumentation is distinct from the
normal C runtime functions under test.

Preserve results, sources, hashes, logs and firmware before any subsequent run,
since each hardware invocation replaces the aggregate results. No timing result
is recorded yet for wide division. Do not advance to step 4 on build results alone.

## Additional audit finding to resolve before closing step 3

The audit found that `__ashldi3`, `__lshrdi3` and `__ashrdi3` retained the
old undeclared return behavior. Their cross-word operation also used a shift by
`32-count`; at count zero, the P2 masks that word shift to zero and incorrectly
ORs the opposite input word into the result. This is inside the stated valid
shift domain (0 through 63). The [follow-up](step3-shifts.md) repairs all three
helpers and adds all-count/word-boundary fixtures. That work also freezes the
old shift dependency of the wide-division timing baseline. `__negdi2` retains
the old return contract and still needs audit. The step-2 `_cnt64` assembly leaf
is unchanged by this series.
