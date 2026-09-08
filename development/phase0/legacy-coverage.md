# Former test/ coverage

The numbered serial-print programs and runner are preserved by the production
baseline tag. They are replaced, rather than kept as a second test framework.
Some former files were interactive demos, benchmarks, or undefined C programs;
those are not silently counted as passing regression tests.

Replacement paths in the table are relative to `tests/` in the repository root.

| Former cases | Replacement / disposition |
|---|---|
| t0–t1: boot and UART | Hardware smoke protocol; separate from host codegen |
| t2–t5: arithmetic, structs, arrays, sizes | codegen/c-patterns.c and focused bug regressions |
| t6–t7, t11: classes, virtual/static members | codegen/cxx-patterns.cpp |
| t8–t10: conditionals, loops, switch | codegen/c-patterns.c |
| t12–t21: calls, arguments, aggregates, recursion | codegen/c-patterns.c, codegen/varargs.c, hardware semantic cases |
| t22: variadic aggregates | codegen/varargs.c and focused aggregate ABI regression |
| t23–t24: registers and waits | Instruction matrix; actual timing requires hardware |
| t25–t26: cog startup | Runtime/hardware integration; not claimed as host execution coverage |
| t28: locks | Instruction matrix covers encodings; contention requires hardware |
| t29: default arguments | codegen/cxx-patterns.cpp |
| t30–t32: large frames, aggregate returns, AUG | codegen/c-patterns.c, MC boundary tests |
| t33: i64 arithmetic | C/IR semantic cases; divide-by-zero expectations removed as undefined C |
| t34: doubles | codegen/float-patterns.c; runtime numeric validation remains hardware work |
| t35: smart-pin interactive demo | Retired as a test; board-specific peripheral validation is separate |
| t36–t38: floating-point experiments/benchmarks | Compiler patterns retained; benchmarks are not correctness oracles |
| t39: experimental cogcache | Retired unsupported-feature experiment; COG residency is a later phase |
| t40: memcpy diagnostic | Memory regression cases; timing/verbose dumps are not pass/fail tests |
| t100: clock/blinker | Retired board-specific demonstration |
| t101: memory/string routines | Hardware semantic suite; compile/ABI tests do not validate libc behavior |
| t102–t103: wide values and volatile loads | C/IR memory and i64 regressions |
| t104–t105: CORDIC experiments | Instruction matrix and explicit CORDIC regressions; latency requires hardware |

This mapping distinguishes migrated host coverage from hardware coverage still
requiring a device. No old test is deleted on the assumption that a generated
opcode check proves equivalent runtime behavior.
