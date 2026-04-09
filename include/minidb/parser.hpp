#pragma once

#include <optional>
#include <string>

#include "minidb/types.hpp"

namespace minidb {

enum class StatementType {
  CreateTable,
  Insert,
  Select,
  Begin,
  Commit,
  Rollback,
  Help,
  Exit,
  Invalid,
};

struct Statement {
  StatementType type{StatementType::Invalid};
  std::string table_name;
  std::optional<Row> row;
  std::optional<Key> where_id;
  std::string error;
};

class Parser {
 public:
  Statement parse(const std::string& input) const;
};

}  // namespace minidb
