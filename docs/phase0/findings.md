# Phase 0 findings and review status

These IDs correspond to the earlier P2 backend review. Changes are on
`phase0_cleanup`; production refs and the original build are retained.
**Host checks and firmware builds are not hardware passes. No board has been run.**

## Likely incorrect assembly / code generation

| ID | Change | Regression / hardware fixture |
|---|---|---|
| B1 | Register-addressed i1 loads use RDBYTErr and mask the low bit | arithmetic-semantics.ll / arithmetic-ir |
| B2 | Signed i64 comparisons use unsigned low-word comparison followed by signed extended high-word comparison | arithmetic-semantics.ll; c-patterns |
| B3 | Signed multiply-high/overflow expands through correct signed legalization instead of unsigned QMUL | arithmetic-semantics.ll; c-patterns |
| B4 | Defined ctlz(0) expands to return 32 | arithmetic-semantics.ll; c-patterns |
| B5 | Unaligned incoming frame offsets retain byte precision | c-patterns, varargs |
| B6 | Aggregate varargs consume their full size; narrow arguments use promoted slot widths; va_arg follows the descending ABI layout | varargs |
| B7 | Short displacements are range/alignment checked; uncertain inline-assembly layout uses long branches | branch-span; operand-boundaries.s |
| B8 | Linker determines LUT execution addresses from the defining section and linker bounds, rather than the callee's spelling | isa-regressions: ordinary HUB sqrt and LUT probe |
| B9 | High, D-low, and S-low relocations preserve full expressions and signed addends | isa-regressions: +528, -1, two augmented fields |
| B10 | Resolved data fixups retain their value and width; unresolved 8/16/32/64-bit data relocations have handlers | isa-regressions: local symbol differences and externally resolved 8/16/32/64-bit values |
| B11 | Forced ## survives parsing; symbolic augmentation is paired; _RET_ does not return from its prefix; numeric HUB addresses 256..511 are distinguished from stack-pointer modes | operand-boundaries.s; isa-regressions; constant-address |
| B12 | Nibble/byte/word operand metadata identifies the correct S and N fields | all-record matrix; scalar ISA SET/GET/ROL fixtures |
| B13 | Pointer syntax, negative indexes, effects, addends, and explicit AUG expressions reassemble | roundtrip; operand-boundaries.s; linked ISA fixtures |
| B14 | Indirect branches have correct operand order, branch properties, and block-address lowering | arithmetic-ir computed branch |
| B15 | Local objects receive alignment slack and aligned addresses even with byte-aligned incoming PTRA; unsupported variable-sized stack allocation gets an explicit diagnostic | stack-alignment, all 32 incoming residues; unsupported-stack.ll |

B15 deliberately does **not** add VLA support. Local alignment costs stack space
and address instructions; review that tradeoff and measure frame/code growth
before using the cleanup compiler in an application with tight resource limits.

The new relocation IDs are appended without renumbering the old ones. The linker
retains guarded support for legacy AUG20 objects, but cannot recover addend bits
already lost by an old assembler. Rebuild objects and the runtime together.
`__p2_lut_load_start/end` describe linker placement; changing the LUT load base
also requires changing the existing startup loader, which still loads from 0x200.

## LLVM API / machine representation

| ID | Change | Validation |
|---|---|---|
| A1 | Correct register def/use construction, block-transfer register operands, and PTRA updates | assertions build, machine verifier, machine-contracts |
| A2 | Pseudo operands match selection, including paired and immediate operands | machine verifier; C/i64 fixtures |
| A3 | Remainder drains QX into a separate virtual register before reading QY | machine-contracts MIR and hardware observations |
| A4 | Fixed incoming frame objects are not inserted into the local frame-object map | MIR printing; incoming-argument cases |
| A5 | cogmain/cogtext/cogcache are target string attributes rather than incomplete global enum extensions | Clang bitcode -> llvm-dis -> llc; cog-attributes firmware uses the bitcode path |
| A6 | Preserve expanded MMOs and transferred registers; describe actual C/Z, QX/QY, FIFO, and CALLA/RETA PTRA effects; bundle SETQ/AUG prefixes with consumers | MIR assertions, verifier, roundtrip, arithmetic and memory fixtures |
| A7 | Correct call chains, return extension and live-in handling; reject invalid named registers; avoid unsafe expression casts | machine-contracts, varargs, invalid-register.ll, linked expressions |

The state changes protect the current pipeline. Adding new target intrinsics,
post-emission transformations, interrupt scheduling, or COG overlays still needs
an explicit state/ABI review; instruction encodings alone are insufficient.

## Additional issues found during replacement testing

- Fresh runtime builds needed repository libc headers and the missing fixdfdi /
  floatdidf helpers. Float conversion fixtures now link from the local build.
- Fixed TESTB/TESTBN/TESTP/TESTPN C/Z opcode selection and rejected invalid effects.
- Removed ADDX patterns that described an ordinary add despite consuming carry.
- Corrected FIFO writes that were described as register definitions.
- Guarded empty/same-address memmove; replaced memset's FIFO/decrement loop with
  completed byte stores, including zero length. Added memory/clobber contracts.
- Made cog startup and lock primitives volatile with compiler memory barriers;
  kept LOCKTRY and WRC in the same inline-assembly block.
- Corrected the counter read sequence to GETCT WC followed by GETCT and returned
  both halves explicitly. Counter reads are volatile.
- Paired the UART debug unlock with the conditional debug lock acquisition.
- Defined lock-allocation failure as ~0u using LOCKNEW's carry result; added
  exhaustion, unique allocation, immediate release/return, and reuse observations.

Runtime cases exercise overlapping moves, empty operations, memcpy block
boundaries, another cog's mailbox work, lock contention, and counter delays.
They are prepared for the chip; their target behavior has not been validated yet.

## Improvements and remaining acceptance work

1. Finish executable instruction coverage. Run `tests/hardware/coverage.py` for
   the exact inventory and gaps; `--require-complete` is an acceptance gate that
   fails while records lack fixtures. Do not describe the structural 368-record
   matrix as 368 hardware tests.
2. Complete independent semantic expectations for remaining stateful/peripheral
   operations. GPIO/loopback, streamer, interrupt, and debug tests need explicit
   setup, observation, cleanup, and a board fixture profile.
3. Execute the complete O0/O2/Os suite on the specified Rev B/C bench board,
   preserving firmware/compiler/runtime hashes and raw observations. Validate
   timeout/failure behavior and review every mismatch before changing an oracle.
4. Measure code size, stack growth, throughput, and UART/clock/interrupt behavior
   against the retained production compiler. No flight validation is implied.
5. Port to the current stable LLVM in a separate series after the semantic
   acceptance gate. See llvm-version-decision.md for the phase 0 decision.
6. Implement multi-function COG residency as a separate feature: HUB bootstrap,
   linker-defined resident groups and capacity checks, explicit load/execution
   addresses, ABI-safe HUB/COG calls, and per-cog residency tracking. The current
   cog-attributes test checks serialization and existing behavior, not this new
   loader/placement feature.

Counter events and LUT-read selectable events now have generated scenarios.
FIFO fixtures run from LUT RAM and drain WRFAST before returning to HUB code.
XORO32 observes both updated state and next-instruction source output using the
Rev B/C algorithm. COG fixtures exercise all four COGINIT operand forms and
both register/immediate worker stops. Remaining gaps are explicitly listed in
the generated coverage report, including SEUSSF/SEUSSR's exact permutation,
GPIO/smart-pin operations, streamer, interrupt and debug behavior. Do not replace
those missing observations with guessed pass values.
