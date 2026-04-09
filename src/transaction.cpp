#include "minidb/transaction.hpp"

namespace minidb {

bool TransactionManager::active() const { return active_; }

bool TransactionManager::begin(std::string& error) {
  if (active_) {
    error = "transaction already active";
    return false;
  }
  active_ = true;
  return true;
}

bool TransactionManager::rollback(std::string& error) {
  if (!active_) {
    error = "no active transaction";
    return false;
  }
  active_ = false;
  return true;
}

}  // namespace minidb
