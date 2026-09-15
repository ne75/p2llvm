# Step 6: backend contracts and cleanup

Step 4's remaining instruction fixtures are deferred until after the cleanup
merge. This checkpoint addresses the current backend and toolchain contracts;
it does not implement additional instruction families or multi-function COG
residency. Production refs and installed production tools remain unchanged.

## Changes to review

- SDK selection now keeps headers, default libraries and linker scripts together.
  Explicit `--sysroot` no longer borrows installed `/opt` archives. Tests perform
  real links against isolated synthetic SDKs, including paths with spaces,
  debugger libraries, library overrides, custom scripts and partial links.
- Normal runtime whole-archive retention remains. `-nostdlib`, `-nodefaultlibs`
  and `-r` now work as documented. `-nostartfiles` receives a diagnostic when
  startup cannot be separated from the default archives.
- Condition/effect positions and augmentable instruction forms share one set
  of metadata helpers across code generation and the assembler. TSFlags layout
  and machine encodings are unchanged.
- Unused COG state, empty hooks, redundant pass wrappers, a nonexistent pass
  declaration and an unused object-file subclass are removed. Duplicate CMake
  entries and an unused emitter generation rule are removed too. The hidden
  `-enable-p2-insert-aug` option is removed because it never controlled the pass. Active
  `cogmain` lowering and all required arithmetic/prefix/flag/branch passes remain.
- The ABI now distinguishes C object alignment from incoming stack alignment,
  defines preserved/return registers and variadic layout, and describes the
  actual COG bootstrap and LUT function-pointer limits.
- Repository policy requires private application work to use temporary storage
  outside this public repository. The published cleanup history was sanitized;
  the retained public benchmark archive contains only generic test evidence.

Start with the [toolchain guide](../../docs/P2%20Toolchain.md) and
[ABI](../../docs/Propeller%202%20ABI.md). The code commits below are separate
review units; root gitlinks point to the corresponding LLVM changes.

## Commit review order

| Repository | Commit | Focus |
|---|---|---|
| Root | `8958fbc` | Public-work policy and deferred work |
| LLVM | `ecbda1fe6c78` | SDK selection and linker behavior |
| Root | `2286588` | Real-link regression tests and driver gitlink |
| Root | `c20b7ee` | Type alignment and memory layout |
| Root | `7bd4e4d` | Calls, returns, varargs and COG startup |
| LLVM | `e2dbb56a4989` | Shared condition/effect/augmentation rules |
| Root | `c4ba0e2` | Metadata gitlink |
| Root | `a996a6e` | Toolchain guide and double-warning limits |
| Root | `ba91832` | Current validation status and coverage deferral |
| LLVM | `a78b9edd6c29` | Unused scaffolding removal |
| Root | `fcccdff` | Validated scaffolding gitlink |

The largest new implementation commit changes 175 lines including deletions.
The largest new ABI update changes 163 lines. These step-6 commits are local for
review; the separately authorized history sanitization has been published.

## Validation

| Check | Result |
|---|---:|
| Instruction encoding/metadata inventory | 368 PASS |
| LLVM MC and CodeGen regressions | 142 PASS |
| Integration tests, including isolated SDK links | 28 PASS |
| Runner, loader, protocol and ISA-oracle unit tests | 14 PASS |
| Portable semantic host runs | 42 PASS; 60 P2-only runs not applicable |
| Rebuilt executable fixture images | 507 / 507 loaded images byte-identical |
| Fresh libc, libp2 and debugger runtime build | PASS |
| Fresh SDK normal/debugger links | 2 PASS |
| Fresh SDK C/C++ benchmark images, O0/O2/Oz | 3 BUILT_NOT_RUN; host oracle PASS |
| Whole-runtime LUT budget at Oz | 1,784 / 2,048 bytes |
| Instruction records with executable fixtures | 294 / 368; remaining 74 deferred |

The [evidence index](step6-checkpoint.json) records source revisions, tool/runtime
hashes and the local archive checksum. The archive includes the compared images,
per-case compile/link logs, software results and SDK build/link artifacts.


The firmware comparison replays the original compile/link commands with the
same fixture sources, run IDs and runtime archive. It compares every loaded
byte after ELF-to-binary conversion, excluding non-loaded ELF provenance.
This is a before/after cleanup check, not a new hardware run. Earlier hardware
checkpoints remain the source of measured semantic and timing results.

## Privacy cleanup limits

Rewriting the published branch removes the private notes from its reachable
history. It does not erase existing clones or guarantee removal from hosting
provider caches. The old local recovery bundle and private evidence are in
system temporary storage outside the repository. The public checkpoint index
references only the sanitized benchmark archive.

## Remaining decisions before merging

- Review the driver compatibility changes, especially SDK selection and
  `-nostartfiles` diagnostics. Custom build systems should explicitly select the
  intended SDK or supply their own script and libraries.
- Accept the documented correctness/performance tradeoffs from earlier steps;
  this cleanup does not remove their alignment or branch-expansion overhead.
- Keep missing peripheral fixtures, multifunction COG residency, LLVM migration,
  stack high-water measurement and comprehensive implicit-double diagnostics in
  [future work](../future-improvements.md).
- Publish the new compiler commits before any root commit referring to them.
  This checkpoint does not itself merge into production.
