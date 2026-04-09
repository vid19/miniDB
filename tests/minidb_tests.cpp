#include <cstdio>
#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "minidb/bplustree.hpp"
#include "minidb/engine.hpp"
#include "minidb/parser.hpp"

namespace {

class TestFailure : public std::runtime_error {
 public:
  explicit TestFailure(const std::string& msg) : std::runtime_error(msg) {}
};

#define ASSERT_TRUE(expr)                                                      \
  do {                                                                         \
    if (!(expr)) {                                                             \
      throw TestFailure(std::string("assertion failed: ") + #expr);          \
    }                                                                          \
  } while (false)

#define ASSERT_EQ(lhs, rhs)                                                    \
  do {                                                                         \
    if (!((lhs) == (rhs))) {                                                   \
      throw TestFailure(std::string("assertion failed: ") + #lhs +           \
                        " == " + #rhs);                                       \
    }                                                                          \
  } while (false)

std::string run(minidb::MiniDBEngine& engine, const std::string& sql) {
  bool should_exit = false;
  std::string result = engine.execute(sql, should_exit);
  ASSERT_TRUE(!should_exit);
  return result;
}

void testBPlusTreeSplitAndScan() {
  minidb::BPlusTree tree;

  tree.upsert(10, "ten");
  tree.upsert(3, "three");
  tree.upsert(8, "eight");
  tree.upsert(1, "one");
  tree.upsert(15, "fifteen");
  tree.upsert(12, "twelve");

  ASSERT_TRUE(tree.contains(8));
  ASSERT_TRUE(!tree.contains(2));

  auto value = tree.find(10);
  ASSERT_TRUE(value.has_value());
  ASSERT_EQ(*value, "ten");

  std::vector<minidb::Row> rows = tree.scan();
  ASSERT_EQ(rows.size(), static_cast<std::size_t>(6));

  for (std::size_t i = 1; i < rows.size(); ++i) {
    ASSERT_TRUE(rows[i - 1].id < rows[i].id);
  }
}

void testParserCommands() {
  minidb::Parser parser;

  auto create_stmt = parser.parse("CREATE TABLE users;");
  ASSERT_EQ(create_stmt.type, minidb::StatementType::CreateTable);
  ASSERT_EQ(create_stmt.table_name, "users");

  auto insert_stmt = parser.parse("INSERT INTO users VALUES (42, 'alice');");
  ASSERT_EQ(insert_stmt.type, minidb::StatementType::Insert);
  ASSERT_TRUE(insert_stmt.row.has_value());
  ASSERT_EQ(insert_stmt.row->id, 42);
  ASSERT_EQ(insert_stmt.row->value, "alice");

  auto select_stmt = parser.parse("SELECT * FROM users WHERE id = 42;");
  ASSERT_EQ(select_stmt.type, minidb::StatementType::Select);
  ASSERT_TRUE(select_stmt.where_id.has_value());
  ASSERT_EQ(*select_stmt.where_id, 42);

  auto begin_stmt = parser.parse("BEGIN;");
  ASSERT_EQ(begin_stmt.type, minidb::StatementType::Begin);

  auto invalid_stmt = parser.parse("totally invalid");
  ASSERT_EQ(invalid_stmt.type, minidb::StatementType::Invalid);
}

void testEngineTransactionsAndPersistence() {
  const auto db_file =
      std::filesystem::temp_directory_path() / "minidb_integration_test.db";
  std::filesystem::remove(db_file);

  minidb::MiniDBEngine engine(db_file);
  std::string init_error;
  ASSERT_TRUE(engine.initialize(init_error));

  ASSERT_EQ(run(engine, "CREATE TABLE users;"), "table created: users");
  ASSERT_EQ(run(engine, "INSERT INTO users VALUES (1, 'Alice');"), "inserted 1 row");

  ASSERT_EQ(run(engine, "BEGIN;"), "transaction started");
  ASSERT_EQ(run(engine, "INSERT INTO users VALUES (2, 'Bob');"),
            "queued 1 row in transaction");

  std::string in_tx = run(engine, "SELECT * FROM users WHERE id = 2;");
  ASSERT_TRUE(in_tx.find("2 | Bob") != std::string::npos);

  ASSERT_EQ(run(engine, "ROLLBACK;"), "transaction rolled back");
  ASSERT_EQ(run(engine, "SELECT * FROM users WHERE id = 2;"), "(0 rows)");

  ASSERT_EQ(run(engine, "BEGIN;"), "transaction started");
  ASSERT_EQ(run(engine, "INSERT INTO users VALUES (3, 'Carol');"),
            "queued 1 row in transaction");
  ASSERT_EQ(run(engine, "COMMIT;"), "transaction committed");

  minidb::MiniDBEngine reloaded(db_file);
  ASSERT_TRUE(reloaded.initialize(init_error));
  std::string persisted = run(reloaded, "SELECT * FROM users WHERE id = 3;");
  ASSERT_TRUE(persisted.find("3 | Carol") != std::string::npos);

  std::filesystem::remove(db_file);
}

}  // namespace

int main() {
  const std::vector<std::pair<std::string, std::function<void()>>> tests = {
      {"B+ tree split and scan", testBPlusTreeSplitAndScan},
      {"Parser commands", testParserCommands},
      {"Engine transactions and persistence", testEngineTransactionsAndPersistence},
  };

  std::size_t passed = 0;
  for (const auto& [name, fn] : tests) {
    try {
      fn();
      ++passed;
      std::cout << "[PASS] " << name << '\n';
    } catch (const std::exception& ex) {
      std::cerr << "[FAIL] " << name << ": " << ex.what() << '\n';
      return 1;
    }
  }

  std::cout << passed << " tests passed\n";
  return 0;
}
