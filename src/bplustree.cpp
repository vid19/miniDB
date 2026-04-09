#include "minidb/bplustree.hpp"

namespace minidb {

bool BPlusTree::contains(Key key) const { return data_.find(key) != data_.end(); }

std::optional<std::string> BPlusTree::find(Key key) const {
  auto it = data_.find(key);
  if (it == data_.end()) {
    return std::nullopt;
  }
  return it->second;
}

void BPlusTree::upsert(Key key, const std::string& value) { data_[key] = value; }

std::vector<Row> BPlusTree::scan() const {
  std::vector<Row> rows;
  rows.reserve(data_.size());
  for (const auto& [id, value] : data_) {
    rows.push_back(Row{id, value});
  }
  return rows;
}

std::size_t BPlusTree::size() const { return data_.size(); }

void BPlusTree::clear() { data_.clear(); }

}  // namespace minidb
