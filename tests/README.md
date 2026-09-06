# P2 test system

Host tests use LLVM's own `lit` and `FileCheck`; no serial device or third-party
Python package is needed. The runner returns a failing exit status and writes
machine-readable JSON results under the selected build directory.

```sh
python3 tests/run.py --build-dir build/phase0-llvm
python3 tests/run.py --suite instructions --build-dir build/phase0-llvm
python3 tests/run.py --suite integration --filter signed
```

## Layers

- **Instruction inventory:** `llvm-tblgen -dump-json` enumerates every concrete
  P2 instruction, derives its operand syntax, checks operand-index metadata,
  assembles it, and compares encoded fields. Fixed opcode bits are independently
  checked against `utils/p2_instructions_revB.csv`. A newly added instruction
  automatically enters this matrix; missing ISA coverage fails explicitly.
- **MC and LLVM regressions:** existing upstream-style tests in
  `llvm-project/llvm/test/{MC,CodeGen}/P2` run through real lit commands.
- **Integration regressions:** this directory contains named C/C++, IR, assembly,
  and ELF-link tests. `%llc` enables `-verify-machineinstrs`; C tests should also
  request machine verification. Tests inspect object bytes and relocations as
  well as assembly when encoding matters.
- **Hardware semantics:** programs and a documented opt-in interface under
  `hardware/` are separate from host tests. Host success does not mean these
  have run. Hardware tests must use bounded execution and explicit result codes.

The generated instruction matrix is structural coverage, not proof that the
TableGen definitions are correct. Keep independently calculated ISA boundary
tests and C semantic expectations. In particular, do not generate expected C
results or every expected opcode from the compiler being tested.

Temporary files use lit's `%t` or the build directory. Tests must not depend on
the current working directory, an installed `/opt/p2llvm`, prior tests, or UART
printing being correct. Add a regression before each fix and keep the fix plus
regression in one reviewable commit.
