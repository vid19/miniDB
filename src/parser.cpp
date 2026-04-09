#include "minidb/parser.hpp"

namespace minidb {

Statement Parser::parse(const std::string& input) const {
  if (input == ".help") {
    return Statement{StatementType::Help, ""};
  }
  if (input == ".exit") {
    return Statement{StatementType::Exit, ""};
  }
  return Statement{StatementType::Invalid, "feature not implemented yet"};
}

}  // namespace minidb
