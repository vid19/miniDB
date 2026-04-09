#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "minidb/types.hpp"

namespace minidb {

class TransactionManager {
 public:
  bool active() const;

  bool begin(std::string& error);
  bool rollback(std::string& error);

  bool stageInsert(const std::string& table_name, const Row& row,
                   std::string& error);

  const std::unordered_map<std::string, std::vector<Row>>& pendingInserts() const;
  std::unordered_map<std::string, std::vector<Row>> commit(std::string& error);

 private:
  bool active_{false};
  std::unordered_map<std::string, std::vector<Row>> pending_inserts_;
};

}  // namespace minidb
