# Step 2: runtime performance review

**Implementation, software checks and targeted hardware validation are complete;
ready for human review.** The [hardware report](step2-hardware.md) records all
results, the corrected benchmark and measured overheads. Step 3 has not started.
The accepted step-1 checkpoint still applies to its recorded runtime hash.

## Review commits

- `3966dec`: restore WRFAST memset, with zero-length handling, write completion,
  declared operands and a 460-case executable fixture (88 additions, 7 deletions).
- `8c335c4`: make _cnt64 a complete assembly leaf returning through R30/R31
  (15 additions, 10 deletions).
- `749a358`: optional benchmarks, frozen comparison implementations and a counter
  rollover fixture (358 additions, 2 deletions).
- `7db2039`: prevent the O2 benchmark from collapsing 64 memset calls into one;
  regression checks the retained loop (29 additions, 1 deletion).

## Runtime changes

Memset stays in LUT RAM and retains the original WFBYTE/DJNZ loop. The zero-length
path does not start the FIFO. A final RDFAST with D[31]=0 waits for pending writes
before return; the source has a compiler memory clobber and an early-clobber
read/write count operand. The optimized function saves/restores only R2, leaves
R0/R1 unchanged and returns the original destination in R31. REP was deliberately
avoided because it defers interrupts while active (Rev B/C v35, printed page 48).
The FIFO completion rule is on printed page 60 of the supplied reference.

_cnt64 now consists of GETCT R31 WC, GETCT R30, RETA at every tested optimization.
The naked attribute makes this a complete assembly function with its own ABI
return, rather than an ordinary C function that falls through without returning
a value. It has no C statements, stack allocation or callee-saved register writes.
No shift helper or C reconstruction is needed.

| Function | Optimization | Previous instructions | Updated instructions |
|---|---|---:|---:|
| memset | O0 | 59 | 67 |
| memset | O2 / Os / Oz | 12 | 10 |
| _cnt64 | O0 | 35 | 3 |
| _cnt64 | O2 / Os / Oz | 18 | 3 |

Counts include the complete function but not callees or dynamic loop iterations.
They are not cycle measurements. Hardware now confirms O0 memset has overhead
versus the original assembly, and Os 8-byte fills cost 8.08 more cycles per call.
O2 fills differ by only five cycles per 64-call batch. See the complete tables
and measurement limits in the hardware report.

## Validation

Recorded software results: 368 instruction checks, 139 LLVM tests, 17 integration
tests, 12 runner/oracle tests; 39 portable executions passed and 27 were not
applicable on the host. All 480 firmware builds passed (157 ordinary/generated
suites plus three optional fixtures, each at O0/O2/Os).

Compiler SHA-256:
`898c4430bad227c208bc933f726925adb7ec198e370aa80d445bdd1a1f5522bc`.
Runtime archive SHA-256:
`3ccd7ae3fb6c0b326590d5d3b3c4b97b3766d7e872f9908d654fc3380ace2699`.

The [hardware README](../../tests/hardware/README.md#runtime-performance-tests)
documents fixture boundaries, metric names, comparison implementations and the
18-image command. It includes the original WRFAST/DJNZ loop with a completion
drain so the comparison measures completed writes, and an empty-call measurement
for loop/call overhead. Performance PASS only confirms valid timing observations;
review the cycle values before accepting performance. The long rollover test
checks a nonzero high word instead of relying solely on readings shortly after reset.

Hardware ran from the user's normal terminal. The initial 18 images passed their
protocol checks, but code inspection invalidated one O2 timing measurement: its
loop had been optimized away. After the benchmark repair, all three memset timing
images passed again with 64 calls verified in linked code. All 21 raw records are
preserved; the accepted set uses the corrected timings. Counter rollover passes
at every optimization and the new counter batches are 2.34–2.83 times faster.
Review the measured short-fill/O0 overhead before advancing to step 3.

## Additional backend findings

Two issues were found while preparing the fixtures and remain open for the
backend-contract audit. They are not hidden by the passing final fixture builds:

- Binding a C unsigned long long inline-assembly output to `r30` accepts the
  declaration but zeroes R31 in optimized output. The pair spelling `r30_r31` is
  rejected by the frontend. The assembly leaf does not depend on this behavior.
- An O2 benchmark with ordinary local min/max arrays unrolled into a long chain
  of selects and failed the machine verifier after post-RA pseudo expansion
  (undefined physical registers). The original source and diagnostic are retained
  with the review evidence. The final harness stores its bookkeeping in volatile
  memory outside timed regions, avoiding that expansion; the compiler is unchanged.
