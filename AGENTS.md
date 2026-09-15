# Repository work

This is a public compiler/runtime repository.

- Keep private application source, binaries, logs, identifying paths, board
  configurations and application-specific reports out of this repository,
  including ignored build directories, evidence archives and commit messages.
- Run private application checks in temporary directories outside the repository.
  Remove temporary copies when testing and any requested manual review are done.
  Do not remove the original application checkout. Keep only self-contained
  public reproductions and generic compiler/runtime conclusions here.
- Make small commits that a person can review independently. Preserve handwritten
  runtime assembly unless compiled code and hardware measurements establish that
  a replacement is at least as performant.
- Keep production baseline refs and installed production tools unchanged during
  cleanup. Build and test in a separate directory.
- Keep reusable documentation in docs/ and development review/results in
  development/. Pair semantic regressions with executable hardware fixtures;
  assembly checks alone do not establish hardware correctness.
