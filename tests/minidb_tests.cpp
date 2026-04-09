#include <stdexcept>
#include <string>

#include "minidb/engine.hpp"

namespace {

void expect(bool cond, const std::string& msg) {
  if (!cond) {
    throw std::runtime_error(msg);
  }
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
  testScaffoldHelpAndExit();
  return 0;
}
