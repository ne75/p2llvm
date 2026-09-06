# Phase 0 review checkpoints

The pre-cleanup production branch is named `master` in this repository. It has
not been renamed or advanced. Work happens on `phase0_cleanup` in the root
repository and the `llvm-project` submodule.

## Production baseline

The root repository, LLVM submodule, and loadp2 submodule each have a
`production_baseline` branch and an annotated `production-baseline-pre-phase0`
tag. Their exact commits and compiler hashes are in
[production-baseline.json](production-baseline.json).

The original `llvm-project/build_release` is retained. A separate local copy of
the compiler tools is in `build/production-baseline/bin`; this ignored binary
snapshot is not distributed by Git. Development builds use `build/phase0-llvm`.

To inspect or return to the production source after saving any ongoing work:

```sh
git switch production_baseline
git submodule update --init --recursive
```

The baseline refs are local until explicitly published. The JSON manifest also
records the submodule commits independently of branch names.

## Reading order

1. [LLVM version decision](llvm-version-decision.md): the stable release checked,
   migration benefits/costs, and the version used for phase 0.
2. `tests/README.md`: test architecture and reproducible commands.
3. `tests/legacy-coverage.md`: disposition of the former numbered test programs.
4. `docs/phase0/findings.md`: review findings, regression tests, and fix status.
5. [Validation checkpoint](validation.md): recorded results, review order, and
   outstanding hardware acceptance work.

Each compiler fix is committed in the LLVM submodule; corresponding root commits
advance its gitlink. Test/build output is kept under `build/`, not checked in as
source. Passing compiler tests does not constitute hardware or flight validation.
