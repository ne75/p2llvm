# Phase 0 validation checkpoint

Recorded September 7, 2026, from the September 6 hardware run and retained rerun.
**Existing hardware semantic suite: PASS under the USB/loader assumption below.**
Broader coverage and performance acceptance remain open. Production
`master`, `production_baseline`, and the baseline tag remain unchanged. All work
is local on `phase0_cleanup`; nothing has been pushed or installed over the
production compiler. The LLVM version decision retains the pinned LLVM 14 fork
for this repair series and separates the subsequent stable-release migration.

## Recorded checks

The compiler was built with assertions, then the runtime was rebuilt from clean
objects using that compiler. The following are recorded results, not tests rerun
for this documentation commit:

| Check | Result |
|---|---|
| TableGen instruction encoding/metadata matrix | 368 passed |
| LLVM P2 MC/CodeGen lit tests | 139 passed |
| Named C/C++/IR/link integration tests | 15 passed |
| Portable semantic fixtures, O0/O2/Os | 36 passed; 27 P2-only runs not applicable on host |
| Runner, loader, observation protocol and scalar oracle unit tests | 12 passed |
| Firmware build, 156 suites × O0/O2/Os | 468 BUILT_NOT_RUN |
| Executable instruction-record fixtures | 294/368 ready; 74 missing |
| Full hardware run, 156 suites × O0/O2/Os | 466 PASS; 2 FAIL (30-second timeouts) |
| Retained targeted hardware rerun | 2 PASS; original timeout verdicts retained |
| Combined semantic checkpoint | 468/468 PASS; two original timeouts treated as assumed USB/loader interruptions |

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
python3 -u tests/hardware/run.py --mode hardware --isa --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000
```

Detailed JSON and logs are in `build/phase0-llvm/p2-test-results` and
`build/phase0-llvm/p2-hardware/{host,build,hardware}`. Each runner invocation
replaces its mode's aggregate results and the selected cases' logs and images.
The preserved checkpoint below is outside those overwrite paths.

## Hardware checkpoint

| Optimization | Passed in full run | Passed in retained rerun | Accepted total |
|---|---:|---:|---:|
| O0 | 156 | 0 | 156 |
| O2 | 156 | 0 | 156 |
| Os | 154 | 2 | 156 |

The full run recorded no semantic mismatches. `c-patterns`, `machine-contracts`,
`runtime-signed-remainder`, `select-compare`, `isa-alts`, and `isa-altd` all passed
at O0/O2/Os after the runtime, select, counter and ALTx fixture corrections.

`cog-init-ii/Os` and `isa-pollct2/Os` timed out with empty captured output.
Both passed in the retained targeted rerun (1.797 and 1.747 seconds respectively).
The user reports three successful rerun invocations; only the latest invocation's
logs remain available. It used the same compiler, runtime, sources and manifest,
but rebuilt firmware with new run IDs, so it was not an identical-binary replay.
All 468 suite/optimization combinations now have an observed pass across the
full run and retained rerun. This is not a single clean 468-run execution.

By user direction, treat these two initial timeouts as USB/loader interruptions
and accept the existing 468-case semantic checkpoint. This is a working assumption
for these two events, not a verified cause or a policy for future failures.
Both original FAIL records remain intact. The runner still has no automatic retry.

Successful logs identify P2 ROM G on `/dev/cu.usbserial-DK0H6QJS`, with detected
clock mode `0x012427f8`. Loading used automatic port selection, RTS, 2 Mbaud,
FIFO 10000 and loader clock defaults. Board model and measured clock frequency
were not supplied; ROM identity alone does not establish those details.

## Preserved evidence

[hardware-checkpoint.json](hardware-checkpoint.json) records the tested root
commit `e2172ec` and LLVM commit `3ceae9f4f4c7`, compiler/runtime/loader/manifest
hashes, rerun identities, and archive sizes and SHA-256 checksums. Compiler and
runtime hashes match across the full run and retained rerun.
Its `semantic_review` field records the accepted disposition and the checksum of
`reviewed-results.json`, a separate derived report in the evidence directory.
That report selects one verified passing attempt for each suite/optimization pair
and retains references to both original timeout records. `reviewed-report.md`
provides the corresponding human-readable summary and complete suite table.

Archives are local, ignored files under
`build/phase0-checkpoints/hardware-2026-09-06/`; they are not included in a clone
or protected by the Git commit. Keep this directory when cleaning build output
and copy it with the review handoff. A separate copy is retained with the local
Codex review artifacts. No remote backup is claimed.

- `p2-hardware-followup-evidence.zip`: complete 468-run results, raw logs,
  firmware, ELF files, fixture sources and merged expectations.
- `p2-hardware-timeout-rerun-evidence.zip`: both retained passing retries,
  including their logs, firmware, sources and expectations.
- `p2-hardware-evidence.zip` and `p2-counter-hardware-evidence.zip`: the initial
  pre-fix full run and the targeted counter-fixture validation.
- `p2-followup-review-evidence.zip`: software results, logs, patches and static
  remainder-assembly comparisons. Its earlier hardware-pending note is historical.

Before recording this checkpoint, archive integrity was checked, all 23,238
observations from the 466 passing full-run logs and all 18 rerun observations
were re-decoded against the expectations, and recorded source/ELF/firmware hashes
were checked against the archived bytes. Failed logs and verdicts are preserved.

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

Step 1 is complete under the documented USB/loader assumption. The next step in
[findings.md](findings.md#agreed-review-sequence) is WRFAST memset and direct-register
counter returns. See [step2.md](step2.md) for the implemented changes and pending
hardware validation of the new runtime; this checkpoint retains the old results.
Performance measurements, remaining runtime audits and 74 instruction fixtures
are still outstanding. Peripheral tests need a defined board and wiring profile.
No production performance or flight acceptance is established by this checkpoint.
The multi-function COG residency feature remains a later series, as designed in
the original backend review; it is not implemented by these repairs.
