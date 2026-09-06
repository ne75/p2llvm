# Phase 0 validation checkpoint

This is a review checkpoint, **not completed hardware acceptance**. Production
`master`, `production_baseline`, and the baseline tag remain unchanged. All work
is local on `phase0_cleanup`; nothing has been pushed or installed over the
production compiler. The LLVM version decision retains the pinned LLVM 14 fork
for this repair series and separates the subsequent stable-release migration.

## Recorded checks

The compiler was built with assertions, then the runtime was rebuilt from clean
objects using that compiler. These commands all completed successfully:

| Check | Result |
|---|---|
| TableGen instruction encoding/metadata matrix | 368 passed |
| LLVM P2 MC/CodeGen lit tests | 138 passed |
| Named C/C++/IR/link integration tests | 14 passed |
| Portable semantic fixtures, O0/O2/Os | 33 passed; 24 P2-only runs not applicable on host |
| Observation protocol and scalar oracle unit tests | 8 passed |
| Firmware build, 154 suites × O0/O2/Os | 462 BUILT_NOT_RUN |
| Executable instruction-record fixtures | 294/368 ready; 74 missing |
| Hardware execution | None |

`coverage.py --require-complete` correctly exits 1 because 74 records lack an
executable fixture. That is an unmet acceptance condition, not a passing test.
The CSV identifies every missing record. Existing fixtures are not exhaustive
for all operands, predicates, timing, or revision-specific corner cases.

```sh
python3 tests/run.py -j4
python3 -m unittest discover -s tests/hardware -p 'test_*.py'
python3 tests/hardware/run.py --mode host
python3 tests/hardware/run.py --mode build --isa
python3 tests/hardware/coverage.py --require-complete
```

Detailed JSON and logs are in `build/phase0-llvm/p2-test-results` and
`build/phase0-llvm/p2-hardware/{host,build}`. Each runner invocation replaces its
mode's aggregate result file; use the full-suite command to capture full-suite
status. Per-case logs and images remain in their case directories.

All 462 builds used compiler SHA-256
`666141b1ccbf2a763619ec7aa39729c68d70a1876bbcf31eed9b80a8b86cdbe5`
and runtime archive SHA-256
`fb35cbed58c0eb2fe396342cab100c54312da692a7b9603677128192ef322508`.
These identify the tested local artifacts; compiler version strings alone do not.

## Human review order

1. `production-baseline.json` and `llvm-version-decision.md`: rollback refs and
   migration sequencing.
2. `tests/README.md`, `tests/legacy-coverage.md`, and the hardware README: what
   replaced the old tests, independent expectations, and acceptance gaps.
3. `findings.md`: B1–B15 and A1–A7, paired regressions, and newly found runtime bugs.
4. Review LLVM commits in order with
   `git -C llvm-project log --reverse --oneline production_baseline..HEAD`, then
   `git -C llvm-project show COMMIT`. Pay particular attention to varargs,
   relocation compatibility, static-local alignment cost, and implicit state.
5. Review root runtime/test commits with
   `git log --reverse --oneline production_baseline..HEAD` and `git show COMMIT`.
   Root gitlink changes point to the separate LLVM commits; inspect those too.

The largest root commit changes 383 lines, including deletions; the largest LLVM
commit changes 358. Runtime fixes, independent oracles, fixture generation, and
old-test removal are separate review units.

## Next acceptance work

Complete exact SEUSSF/SEUSSR expectations and the remaining peripheral,
streamer, interrupt and debug fixtures. Hardware work needs a bench board ID,
serial port, clock mode/frequency, and permitted GPIO/loopback wiring. Then run
the full suite on Rev B/C silicon and investigate every mismatch. Measure code
size, stack growth and timing against production before promoting any compiler.
The multi-function COG residency feature remains a later series, as designed in
the original backend review; it is not implemented by these repairs.
