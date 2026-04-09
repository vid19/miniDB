#include "minidb/table.hpp"

#include <utility>

namespace minidb {

Table::Table() = default;

Table::Table(std::string name) : name_(std::move(name)) {}

const std::string& Table::name() const { return name_; }

bool Table::insert(const Row& row, std::string* error) {
  if (index_.contains(row.id)) {
    if (error != nullptr) {
      *error = "duplicate primary key: " + std::to_string(row.id);
    }
    return false;
  }

  index_.upsert(row.id, row.value);
  return true;
}

void Table::upsertForRecovery(const Row& row) { index_.upsert(row.id, row.value); }

std::optional<Row> Table::select(Key id) const {
  auto value = index_.find(id);
  if (!value.has_value()) {
    return std::nullopt;
  }

  return Row{id, *value};
}

std::vector<Row> Table::selectAll() const { return index_.scan(); }

bool Table::contains(Key id) const { return index_.contains(id); }

std::size_t Table::size() const { return index_.size(); }

}  // namespace minidb
