#include "minidb/engine.hpp"

#include <utility>

namespace minidb {

MiniDBEngine::MiniDBEngine(std::filesystem::path db_file)
    : storage_(std::move(db_file)) {}

bool MiniDBEngine::initialize(std::string& error) {
  error.clear();
  static_cast<void>(storage_.load());
  return true;
}

std::string MiniDBEngine::execute(const std::string& sql, bool& should_exit) {
  should_exit = false;

  const Statement stmt = parser_.parse(sql);
  if (stmt.type == StatementType::Help) {
    return "commands: .help, .exit";
  }
  if (stmt.type == StatementType::Exit) {
    should_exit = true;
    return "bye";
  }
  return "error: " + stmt.error;
}

}  // namespace minidb
