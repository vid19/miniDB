# miniDB (SQLite-like in C++)

A local database engine project built in modern C++.

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Project Goals

- B+ tree indexing
- SQL-like parser
- query execution engine
- transactions (`BEGIN`/`COMMIT`/`ROLLBACK`)
- durable file persistence

## PR Workflow

See [`CONTRIBUTING.md`](CONTRIBUTING.md) and [`docs/ROADMAP.md`](docs/ROADMAP.md).
