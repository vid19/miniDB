#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "minidb/types.hpp"

namespace minidb {

class BPlusTree {
 public:
  BPlusTree();

  bool contains(Key key) const;
  std::optional<std::string> find(Key key) const;

  void upsert(Key key, const std::string& value);
  std::vector<Row> scan() const;

  std::size_t size() const;
  void clear();

 private:
  static constexpr std::size_t kMaxKeys = 4;

  struct Node {
    explicit Node(bool is_leaf);

    bool is_leaf;
    std::vector<Key> keys;
    std::vector<std::shared_ptr<Node>> children;
    std::vector<std::string> values;
    std::shared_ptr<Node> next;
  };

  struct InsertResult {
    bool split{false};
    Key pivot{};
    std::shared_ptr<Node> right;
  };

  std::shared_ptr<Node> root_;
  std::size_t size_{0};

  static std::size_t childIndex(const std::vector<Key>& keys, Key key);
  std::shared_ptr<Node> leftMostLeaf() const;

  InsertResult insertRecursive(const std::shared_ptr<Node>& node, Key key,
                               const std::string& value, bool& inserted_new);
};

}  // namespace minidb
