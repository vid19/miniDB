#include "minidb/transaction.hpp"

namespace minidb {

bool TransactionManager::active() const { return active_; }

bool TransactionManager::begin(std::string& error) {
  if (active_) {
    error = "transaction already active";
    return false;
  }

  active_ = true;
  pending_inserts_.clear();
  return true;
}

bool TransactionManager::rollback(std::string& error) {
  if (!active_) {
    error = "no active transaction";
    return false;
  }

  active_ = false;
  pending_inserts_.clear();
  return true;
}

bool TransactionManager::stageInsert(const std::string& table_name, const Row& row,
                                     std::string& error) {
  if (!active_) {
    error = "no active transaction";
    return false;
  }

  pending_inserts_[table_name].push_back(row);
  return true;
}

const std::unordered_map<std::string, std::vector<Row>>&
TransactionManager::pendingInserts() const {
  return pending_inserts_;
}

std::unordered_map<std::string, std::vector<Row>> TransactionManager::commit(
    std::string& error) {
  if (!active_) {
    error = "no active transaction";
    return {};
  }

  active_ = false;
  auto staged = std::move(pending_inserts_);
  pending_inserts_.clear();
  return staged;
}

}  // namespace minidb
