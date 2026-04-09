#include <iostream>
#include <string>

#include "minidb/engine.hpp"

int main() {
  minidb::MiniDBEngine engine("data/minidb.db");
  std::string error;
  if (!engine.initialize(error)) {
    std::cerr << "failed to initialize: " << error << '\n';
    return 1;
  }

  std::cout << "miniDB scaffold (.help, .exit)\n";
  std::string line;
  while (true) {
    std::cout << "miniDB> ";
    if (!std::getline(std::cin, line)) {
      break;
    }

    bool should_exit = false;
    std::cout << engine.execute(line, should_exit) << '\n';
    if (should_exit) {
      break;
    }
  }

  return 0;
}
