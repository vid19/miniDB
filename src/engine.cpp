#include "minidb/engine.hpp"

#include <algorithm>
#include <sstream>

namespace minidb {

MiniDBEngine::MiniDBEngine(std::filesystem::path db_file) : storage_(std::move(db_file)) {}

bool MiniDBEngine::initialize(std::string& error) {
  try {
    tables_ = storage_.load();
  } catch (const std::exception& ex) {
    error = ex.what();
    return false;
  }

  return true;
}

std::string MiniDBEngine::execute(const std::string& sql, bool& should_exit) {
  should_exit = false;

  Statement stmt = parser_.parse(sql);
  switch (stmt.type) {
    case StatementType::CreateTable:
      return handleCreateTable(stmt);
    case StatementType::Insert:
      return handleInsert(stmt);
    case StatementType::Select:
      return handleSelect(stmt);
    case StatementType::Begin:
      return handleBegin();
    case StatementType::Commit:
      return handleCommit();
    case StatementType::Rollback:
      return handleRollback();
    case StatementType::Help:
      return helpText();
    case StatementType::Exit:
      should_exit = true;
      return "bye";
    case StatementType::Invalid:
    default:
      return "error: " + stmt.error;
  }
}

std::string MiniDBEngine::handleCreateTable(const Statement& stmt) {
  if (txn_.active()) {
    return "error: cannot create table inside an active transaction";
  }

  if (tables_.find(stmt.table_name) != tables_.end()) {
    return "error: table already exists: " + stmt.table_name;
  }

  tables_.emplace(stmt.table_name, Table(stmt.table_name));
  try {
    storage_.save(tables_);
  } catch (const std::exception& ex) {
    return std::string("error: failed to persist table: ") + ex.what();
  }

  return "table created: " + stmt.table_name;
}

std::string MiniDBEngine::handleInsert(const Statement& stmt) {
  auto table_it = tables_.find(stmt.table_name);
  if (table_it == tables_.end()) {
    return "error: unknown table: " + stmt.table_name;
  }

  if (!stmt.row.has_value()) {
    return "error: invalid insert payload";
  }

  const Row row = *stmt.row;

  if (txn_.active()) {
    if (table_it->second.contains(row.id)) {
      return "error: duplicate primary key: " + std::to_string(row.id);
    }

    auto pending_it = txn_.pendingInserts().find(stmt.table_name);
    if (pending_it != txn_.pendingInserts().end()) {
      const auto& pending_rows = pending_it->second;
      auto duplicate_it = std::find_if(
          pending_rows.begin(), pending_rows.end(),
          [row](const Row& pending_row) { return pending_row.id == row.id; });
      if (duplicate_it != pending_rows.end()) {
        return "error: duplicate primary key in transaction: " +
               std::to_string(row.id);
      }
    }

    std::string stage_error;
    if (!txn_.stageInsert(stmt.table_name, row, stage_error)) {
      return "error: " + stage_error;
    }

    return "queued 1 row in transaction";
  }

  std::string insert_error;
  if (!table_it->second.insert(row, &insert_error)) {
    return "error: " + insert_error;
  }

  try {
    storage_.save(tables_);
  } catch (const std::exception& ex) {
    return std::string("error: failed to persist insert: ") + ex.what();
  }

  return "inserted 1 row";
}

std::string MiniDBEngine::handleSelect(const Statement& stmt) const {
  auto table_it = tables_.find(stmt.table_name);
  if (table_it == tables_.end()) {
    return "error: unknown table: " + stmt.table_name;
  }

  if (stmt.where_id.has_value()) {
    const Key id = *stmt.where_id;

    if (txn_.active()) {
      auto pending_it = txn_.pendingInserts().find(stmt.table_name);
      if (pending_it != txn_.pendingInserts().end()) {
        for (const Row& row : pending_it->second) {
          if (row.id == id) {
            return std::to_string(row.id) + " | " + row.value + "\n(1 row)";
          }
        }
      }
    }

    auto row = table_it->second.select(id);
    if (!row.has_value()) {
      return "(0 rows)";
    }

    return std::to_string(row->id) + " | " + row->value + "\n(1 row)";
  }

  std::vector<Row> rows = table_it->second.selectAll();

  if (txn_.active()) {
    auto pending_it = txn_.pendingInserts().find(stmt.table_name);
    if (pending_it != txn_.pendingInserts().end()) {
      rows.insert(rows.end(), pending_it->second.begin(), pending_it->second.end());
      std::sort(rows.begin(), rows.end(),
                [](const Row& lhs, const Row& rhs) { return lhs.id < rhs.id; });
    }
  }

  if (rows.empty()) {
    return "(0 rows)";
  }

  std::ostringstream out;
  out << "id | value\n";
  out << "-----------\n";
  for (const Row& row : rows) {
    out << row.id << " | " << row.value << "\n";
  }

  out << "(" << rows.size() << (rows.size() == 1 ? " row)" : " rows)");
  return out.str();
}

std::string MiniDBEngine::handleBegin() {
  std::string error;
  if (!txn_.begin(error)) {
    return "error: " + error;
  }

  return "transaction started";
}

std::string MiniDBEngine::handleCommit() {
  std::string commit_error;
  auto staged = txn_.commit(commit_error);
  if (!commit_error.empty()) {
    return "error: " + commit_error;
  }

  for (auto& [table_name, rows] : staged) {
    auto table_it = tables_.find(table_name);
    if (table_it == tables_.end()) {
      return "error: unknown table in staged transaction: " + table_name;
    }

    for (const Row& row : rows) {
      std::string insert_error;
      if (!table_it->second.insert(row, &insert_error)) {
        return "error: commit failed: " + insert_error;
      }
    }
  }

  try {
    storage_.save(tables_);
  } catch (const std::exception& ex) {
    return std::string("error: failed to persist commit: ") + ex.what();
  }

  return "transaction committed";
}

std::string MiniDBEngine::handleRollback() {
  std::string error;
  if (!txn_.rollback(error)) {
    return "error: " + error;
  }

  return "transaction rolled back";
}

std::string MiniDBEngine::helpText() const {
  return "commands:\n"
         "  CREATE TABLE <name>;\n"
         "  INSERT INTO <name> VALUES (<id>, '<value>');\n"
         "  SELECT * FROM <name>;\n"
         "  SELECT * FROM <name> WHERE id = <id>;\n"
         "  BEGIN;\n"
         "  COMMIT;\n"
         "  ROLLBACK;\n"
         "  .help\n"
         "  .exit";
}

}  // namespace minidb
