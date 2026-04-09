#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "minidb/bplustree.hpp"
#include "minidb/engine.hpp"
#include "minidb/storage.hpp"
#include "minidb/table.hpp"

namespace {

void expect(bool cond, const std::string& msg) {
  if (!cond) {
    throw std::runtime_error(msg);
  }
}

void testBPlusTreeSplitAndScan() {
  minidb::BPlusTree tree;

  tree.upsert(10, "ten");
  tree.upsert(3, "three");
  tree.upsert(8, "eight");
  tree.upsert(1, "one");
  tree.upsert(15, "fifteen");
  tree.upsert(12, "twelve");

  expect(tree.contains(8), "tree should contain key 8");
  expect(!tree.contains(2), "tree should not contain key 2");

  auto value = tree.find(10);
  expect(value.has_value(), "key 10 should exist");
  expect(*value == "ten", "value for key 10 should match");

  std::vector<minidb::Row> rows = tree.scan();
  expect(rows.size() == 6, "tree should contain 6 rows");

  for (std::size_t i = 1; i < rows.size(); ++i) {
    expect(rows[i - 1].id < rows[i].id, "scan should be sorted");
  }
}

void testPersistenceRoundTrip() {
  const auto db_file =
      std::filesystem::temp_directory_path() / "minidb_persistence_test.db";
  std::filesystem::remove(db_file);

  std::unordered_map<std::string, minidb::Table> tables;
  tables.emplace("users", minidb::Table("users"));

  std::string error;
  expect(tables.at("users").insert(minidb::Row{1, "Alice"}, &error),
         "insert should succeed");
  expect(tables.at("users").insert(minidb::Row{2, "Bob"}, &error),
         "insert should succeed");

  minidb::StorageManager storage(db_file);
  storage.save(tables);

  auto loaded = storage.load();
  expect(loaded.find("users") != loaded.end(), "users table should be restored");

  auto row = loaded.at("users").select(2);
  expect(row.has_value(), "row id=2 should exist after load");
  expect(row->value == "Bob", "restored value should match");

  std::filesystem::remove(db_file);
}

void testScaffoldHelpAndExit() {
  minidb::MiniDBEngine engine("/tmp/minidb_scaffold.db");
  std::string error;
  expect(engine.initialize(error), "engine should initialize");

  bool should_exit = false;
  auto help = engine.execute(".help", should_exit);
  expect(help.find(".help") != std::string::npos, "help should mention .help");
  expect(!should_exit, "help should not exit");

  auto bye = engine.execute(".exit", should_exit);
  expect(bye == "bye", "exit should return bye");
  expect(should_exit, "exit should mark should_exit");
}

}  // namespace

int main() {
  testBPlusTreeSplitAndScan();
  testPersistenceRoundTrip();
  testScaffoldHelpAndExit();
  return 0;
}
