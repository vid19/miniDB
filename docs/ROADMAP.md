# Feature Roadmap (Push Per Feature)

Use this sequence to create clean resume-friendly PRs:

1. `feature/scaffold-ci`
- Setup CMake, source layout, tests, CI, and PR template.

2. `feature/bplustree-index`
- Implement B+ tree insert/search/scan with split logic.

3. `feature/persistence-layer`
- Add file format, save/load, and crash-safe parsing behavior.

4. `feature/sql-parser`
- Parse SQL-like statements into typed commands.

5. `feature/query-executor`
- Execute parsed statements over table/index abstractions.

6. `feature/transactions`
- Stage writes and support `BEGIN/COMMIT/ROLLBACK`.

7. `feature/e2e-tests-cli`
- Add integration tests and improve CLI UX.

## Repeated Workflow

```bash
git checkout main
git pull
git checkout -b feature/<name>
# build the feature
cmake -S . -B build && cmake --build build && ctest --test-dir build

git add .
git commit -m "feat(...): ..."
git push -u origin feature/<name>
# open PR
```
