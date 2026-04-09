#pragma once

#include <string>

namespace minidb {

class TransactionManager {
 public:
  bool active() const;
  bool begin(std::string& error);
  bool rollback(std::string& error);

 private:
  bool active_{false};
};

}  // namespace minidb
