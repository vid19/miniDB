# miniDB (SQLite-like in C++)

A local database engine built in modern C++ with:
- B+ tree indexing
- SQL-like query parsing
- transaction support (`BEGIN`, `COMMIT`, `ROLLBACK`)
- durable file persistence

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

```bash
./build/minidb_cli
```

Optional custom DB file:

```bash
./build/minidb_cli /path/to/my.db
```

## Supported SQL-like Commands

- `CREATE TABLE users;`
- `INSERT INTO users VALUES (1, 'Alice');`
- `SELECT * FROM users;`
- `SELECT * FROM users WHERE id = 1;`
- `BEGIN;`
- `COMMIT;`
- `ROLLBACK;`
- `.help`
- `.exit`

## Architecture

- `BPlusTree` in-memory index for row IDs.
- `Table` wraps the index and row semantics.
- `StorageManager` serializes and restores data from disk.
- `Parser` turns SQL-like text into typed statements.
- `TransactionManager` tracks staged inserts in active transactions.
- `MiniDBEngine` coordinates parser + tables + storage + transactions.
