#include "minidb/engine.hpp"

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

  std::string insert_error;
  if (!table_it->second.insert(*stmt.row, &insert_error)) {
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
    auto row = table_it->second.select(*stmt.where_id);
    if (!row.has_value()) {
      return "(0 rows)";
    }

    return std::to_string(row->id) + " | " + row->value + "\n(1 row)";
  }

  const auto rows = table_it->second.selectAll();
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

std::string MiniDBEngine::helpText() const {
  return "commands:\n"
         "  CREATE TABLE <name>;\n"
         "  INSERT INTO <name> VALUES (<id>, '<value>');\n"
         "  SELECT * FROM <name>;\n"
         "  SELECT * FROM <name> WHERE id = <id>;\n"
         "  .help\n"
         "  .exit";
}

}  // namespace minidb
