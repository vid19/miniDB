# Contributing and PR Rules

## Branching

- Main branch: `main`
- Feature branches: `feature/<short-description>`
- Bug fix branches: `fix/<short-description>`

## Feature Delivery Rules

- Build one feature per branch.
- Open one PR per feature.
- Keep PRs small and reviewable (< 400 lines preferred).
- Every PR must include tests for behavior changes.

## Commit Rules

- Use clear commit messages:
  - `feat(index): implement leaf split in b+ tree`
  - `feat(parser): add create/insert/select parsing`
  - `feat(txn): add begin/commit/rollback`
  - `test(engine): add transaction integration tests`

## PR Checklist

- `cmake --build build` passes
- `ctest --test-dir build` passes
- no accidental file format changes
- docs updated when behavior changes

## Suggested Feature Sequence

1. project scaffold + CI + docs
2. B+ tree indexing
3. storage layer + persistence format
4. query parser
5. execution engine
6. transaction manager
7. integration tests + CLI polish
