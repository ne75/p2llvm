# LLVM version decision

## Decision for phase0_cleanup

Keep the current pinned LLVM 14 development revision for the test replacement
and correctness repair series. Target **LLVM 23.1.0** for the subsequent migration
series, subject to rechecking the latest stable patch release when that series
starts. Do not use upstream development `main` as a production target.

This is a sequencing decision, not an endorsement of staying on LLVM 14
indefinitely. The current compiler reports 14.0.0 but is a forked development
revision, not the final upstream 14.0.6 release.

## Release checked

LLVM announced 23.1.0 on August 26, 2026. The next patch, 23.1.1, is scheduled for
September 8. As checked for this work, 23.1.0 is the released stable target.
[Release announcement](https://discourse.llvm.org/t/llvm-23-1-0-released/91654)

## Reasons to upgrade

- Modern Clang language support and accumulated frontend/middle-end fixes.
- Compatibility with contemporary LLVM IR producers and maintenance tools.
- A maintained upstream baseline reduces the growing cost of carrying the P2
  target, Clang integration, and ELF linker implementation in an old fork.
- Current infrastructure improvements can benefit the toolchain, but a P2 code
  size or speed improvement must be measured; it is not guaranteed by upgrading.

## Reasons to repair and measure the current version first

- The review already reproduces wrong encodings, ABI errors, broken COG
  attribute bitcode, and machine-verifier failures. Those are P2 implementation
  defects, and changing LLVM does not establish that they are fixed.
- The flight-used baseline and current tool binaries can serve as an explicit
  comparison point. Mixing a major upstream port with each bug fix obscures
  regressions and makes individual commits harder to review or bisect.
- The old tests use typed pointers. LLVM 17 and later support only opaque
  pointers, so the migration includes IR fixtures and any target-specific
  assumptions about pointee types, not just updating a submodule pointer.
  [Opaque-pointer migration](https://llvm.org/docs/OpaquePointers.html)
- LLVM 23 also changes register-class APIs and TableGen syntax. The port must
  audit target registration, calling-convention hooks, MC/parser/printer APIs,
  attributes/intrinsics, and lld relocation integration against the actual new
  source. [LLVM 23 release notes](https://releases.llvm.org/23.1.0/docs/ReleaseNotes.html)

## Migration gate and deliverables

First make the host test suite, machine verification, assembly round trips, and
link regression tests pass on phase 0. Then port the tested P2 changes to a pinned
stable LLVM release in a separate commit series, preserving the phase-0 branch.
Convert IR fixtures explicitly, rebuild the runtime, compare code size and
semantic results at O0/O2/Os, and run the hardware suite before replacing the
production toolchain. Keep ABI changes independently documented and tested.

No current P2 requirement identified in the review requires an immediate LLVM
version change. Multi-function COG residency can be designed on either baseline;
it is not a reason to combine the port with the initial correctness work.
