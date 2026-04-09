#pragma once

#include <string>

namespace minidb {

enum class StatementType {
  Help,
  Exit,
  Invalid,
};

struct Statement {
  StatementType type{StatementType::Invalid};
  std::string error;
};

class Parser {
 public:
  Statement parse(const std::string& input) const;
};

}  // namespace minidb
