#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "minidb/types.hpp"

namespace minidb {

class BPlusTree {
 public:
  bool contains(Key key) const;
  std::optional<std::string> find(Key key) const;
  void upsert(Key key, const std::string& value);
  std::vector<Row> scan() const;
  std::size_t size() const;
  void clear();

 private:
  std::map<Key, std::string> data_;
};

}  // namespace minidb
