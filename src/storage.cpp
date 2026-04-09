#include "minidb/storage.hpp"

namespace minidb {

StorageManager::StorageManager(std::filesystem::path file_path)
    : file_path_(std::move(file_path)) {}

void StorageManager::save(const std::unordered_map<std::string, Table>&) const {
  // persistence arrives in a dedicated feature PR
}

std::unordered_map<std::string, Table> StorageManager::load() const {
  // persistence arrives in a dedicated feature PR
  return {};
}

}  // namespace minidb
