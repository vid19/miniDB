#pragma once

#include <optional>
#include <string>
#include <vector>

#include "minidb/bplustree.hpp"
#include "minidb/types.hpp"

namespace minidb {

class Table {
 public:
  Table();
  explicit Table(std::string name);

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;
  Table(Table&&) noexcept = default;
  Table& operator=(Table&&) noexcept = default;

  const std::string& name() const;

  bool insert(const Row& row, std::string* error);
  void upsertForRecovery(const Row& row);

  std::optional<Row> select(Key id) const;
  std::vector<Row> selectAll() const;

  bool contains(Key id) const;
  std::size_t size() const;

 private:
  std::string name_;
  BPlusTree index_;
};

}  // namespace minidb
