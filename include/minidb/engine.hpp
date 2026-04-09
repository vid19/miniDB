#pragma once

#include <filesystem>
#include <string>

#include "minidb/parser.hpp"
#include "minidb/storage.hpp"
#include "minidb/transaction.hpp"

namespace minidb {

class MiniDBEngine {
 public:
  explicit MiniDBEngine(std::filesystem::path db_file);

  bool initialize(std::string& error);
  std::string execute(const std::string& sql, bool& should_exit);

 private:
  Parser parser_;
  StorageManager storage_;
  TransactionManager txn_;
};

}  // namespace minidb
