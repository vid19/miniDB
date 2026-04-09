#pragma once

#include <filesystem>
#include <unordered_map>

#include "minidb/table.hpp"

namespace minidb {

class StorageManager {
 public:
  explicit StorageManager(std::filesystem::path file_path);

  void save(const std::unordered_map<std::string, Table>& tables) const;
  std::unordered_map<std::string, Table> load() const;

 private:
  std::filesystem::path file_path_;
};

}  // namespace minidb
