#include "minidb/parser.hpp"

#include <regex>

namespace minidb {

Statement Parser::parse(const std::string& input) const {
  Statement stmt;

  if (input.empty()) {
    stmt.error = "empty input";
    return stmt;
  }

  if (input == ".help") {
    stmt.type = StatementType::Help;
    return stmt;
  }

  if (input == ".exit") {
    stmt.type = StatementType::Exit;
    return stmt;
  }

  const std::regex create_re(
      R"(^\s*CREATE\s+TABLE\s+([A-Za-z_][A-Za-z0-9_]*)\s*;?\s*$)",
      std::regex::icase);
  const std::regex insert_re(
      R"(^\s*INSERT\s+INTO\s+([A-Za-z_][A-Za-z0-9_]*)\s+VALUES\s*\(\s*(-?\d+)\s*,\s*'([^']*)'\s*\)\s*;?\s*$)",
      std::regex::icase);
  const std::regex select_re(
      R"(^\s*SELECT\s+\*\s+FROM\s+([A-Za-z_][A-Za-z0-9_]*)\s*(WHERE\s+id\s*=\s*(-?\d+)\s*)?;?\s*$)",
      std::regex::icase);
  const std::regex begin_re(R"(^\s*BEGIN\s*;?\s*$)", std::regex::icase);
  const std::regex commit_re(R"(^\s*COMMIT\s*;?\s*$)", std::regex::icase);
  const std::regex rollback_re(R"(^\s*ROLLBACK\s*;?\s*$)", std::regex::icase);

  std::smatch match;

  if (std::regex_match(input, match, create_re)) {
    stmt.type = StatementType::CreateTable;
    stmt.table_name = match[1].str();
    return stmt;
  }

  if (std::regex_match(input, match, insert_re)) {
    stmt.type = StatementType::Insert;
    stmt.table_name = match[1].str();
    stmt.row = Row{std::stoll(match[2].str()), match[3].str()};
    return stmt;
  }

  if (std::regex_match(input, match, select_re)) {
    stmt.type = StatementType::Select;
    stmt.table_name = match[1].str();
    if (match[3].matched) {
      stmt.where_id = std::stoll(match[3].str());
    }
    return stmt;
  }

  if (std::regex_match(input, begin_re)) {
    stmt.type = StatementType::Begin;
    return stmt;
  }

  if (std::regex_match(input, commit_re)) {
    stmt.type = StatementType::Commit;
    return stmt;
  }

  if (std::regex_match(input, rollback_re)) {
    stmt.type = StatementType::Rollback;
    return stmt;
  }

  stmt.error = "could not parse statement";
  return stmt;
}

}  // namespace minidb
