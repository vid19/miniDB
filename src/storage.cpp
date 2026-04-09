#include "minidb/storage.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace minidb {

StorageManager::StorageManager(std::filesystem::path file_path)
    : file_path_(std::move(file_path)) {}

void StorageManager::save(
    const std::unordered_map<std::string, Table>& tables) const {
  if (!file_path_.parent_path().empty()) {
    std::filesystem::create_directories(file_path_.parent_path());
  }

  std::ofstream out(file_path_, std::ios::trunc);
  if (!out) {
    throw std::runtime_error("failed to open DB file for writing: " +
                             file_path_.string());
  }

  out << "MINIDB 1\n";
  for (const auto& [table_name, table] : tables) {
    out << "TABLE " << std::quoted(table_name) << "\n";

    for (const Row& row : table.selectAll()) {
      out << "ROW " << row.id << " " << std::quoted(row.value) << "\n";
    }

    out << "END_TABLE\n";
  }
}

std::unordered_map<std::string, Table> StorageManager::load() const {
  std::unordered_map<std::string, Table> tables;

  if (!std::filesystem::exists(file_path_)) {
    return tables;
  }

  std::ifstream in(file_path_);
  if (!in) {
    throw std::runtime_error("failed to open DB file for reading: " +
                             file_path_.string());
  }

  std::string line;
  if (!std::getline(in, line)) {
    return tables;
  }

  if (line != "MINIDB 1") {
    throw std::runtime_error("unsupported DB file format");
  }

  std::string current_table;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "TABLE") {
      std::string table_name;
      iss >> std::quoted(table_name);
      current_table = table_name;
      tables.erase(current_table);
      tables.emplace(current_table, Table(current_table));
      continue;
    }

    if (token == "ROW") {
      if (current_table.empty()) {
        throw std::runtime_error("ROW found outside TABLE section");
      }

      Key id = 0;
      std::string value;
      iss >> id >> std::quoted(value);

      auto table_it = tables.find(current_table);
      if (table_it == tables.end()) {
        throw std::runtime_error("internal load error: table missing");
      }

      table_it->second.upsertForRecovery(Row{id, value});
      continue;
    }

    if (token == "END_TABLE") {
      current_table.clear();
      continue;
    }

    throw std::runtime_error("unknown token in DB file: " + token);
  }

  return tables;
}

const std::filesystem::path& StorageManager::filePath() const { return file_path_; }

}  // namespace minidb
