#include <iostream>
#include <string>

#include "minidb/engine.hpp"

int main(int argc, char** argv) {
  std::string db_file = "data/minidb.db";
  if (argc > 1 && argv[1] != nullptr) {
    db_file = argv[1];
  }

  minidb::MiniDBEngine engine(db_file);
  std::string init_error;
  if (!engine.initialize(init_error)) {
    std::cerr << "failed to initialize miniDB: " << init_error << '\n';
    return 1;
  }

  std::cout << "miniDB CLI (.help for commands, .exit to quit)\n";

  std::string line;
  while (true) {
    std::cout << "miniDB> ";
    if (!std::getline(std::cin, line)) {
      std::cout << '\n';
      break;
    }

    if (line.empty()) {
      continue;
    }

    bool should_exit = false;
    const std::string result = engine.execute(line, should_exit);
    std::cout << result << '\n';

    if (should_exit) {
      break;
    }
  }

  return 0;
}
