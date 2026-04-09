#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "minidb/parser.hpp"
#include "minidb/storage.hpp"
#include "minidb/table.hpp"

namespace minidb {

class MiniDBEngine {
 public:
  explicit MiniDBEngine(std::filesystem::path db_file);

  bool initialize(std::string& error);
  std::string execute(const std::string& sql, bool& should_exit);

 private:
  Parser parser_;
  StorageManager storage_;
  std::unordered_map<std::string, Table> tables_;

  std::string handleCreateTable(const Statement& stmt);
  std::string handleInsert(const Statement& stmt);
  std::string handleSelect(const Statement& stmt) const;

  std::string helpText() const;
};

}  // namespace minidb
