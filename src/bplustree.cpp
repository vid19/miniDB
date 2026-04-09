#include "minidb/bplustree.hpp"

#include <algorithm>
#include <utility>

namespace minidb {

BPlusTree::Node::Node(bool is_leaf) : is_leaf(is_leaf) {}

BPlusTree::BPlusTree() : root_(std::make_shared<Node>(true)) {}

std::size_t BPlusTree::childIndex(const std::vector<Key>& keys, Key key) {
  return static_cast<std::size_t>(
      std::upper_bound(keys.begin(), keys.end(), key) - keys.begin());
}

bool BPlusTree::contains(Key key) const { return find(key).has_value(); }

std::optional<std::string> BPlusTree::find(Key key) const {
  auto node = root_;
  while (node != nullptr && !node->is_leaf) {
    const std::size_t next = childIndex(node->keys, key);
    node = node->children[next];
  }

  if (node == nullptr) {
    return std::nullopt;
  }

  auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
  if (it == node->keys.end() || *it != key) {
    return std::nullopt;
  }

  const std::size_t idx = static_cast<std::size_t>(it - node->keys.begin());
  return node->values[idx];
}

BPlusTree::InsertResult BPlusTree::insertRecursive(const std::shared_ptr<Node>& node,
                                                   Key key,
                                                   const std::string& value,
                                                   bool& inserted_new) {
  if (node->is_leaf) {
    auto key_it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
    const std::size_t idx = static_cast<std::size_t>(key_it - node->keys.begin());

    if (key_it != node->keys.end() && *key_it == key) {
      node->values[idx] = value;
      return {};
    }

    node->keys.insert(key_it, key);
    node->values.insert(node->values.begin() + static_cast<std::ptrdiff_t>(idx),
                        value);
    inserted_new = true;

    if (node->keys.size() <= kMaxKeys) {
      return {};
    }

    const std::size_t split_idx = node->keys.size() / 2;
    auto right = std::make_shared<Node>(true);

    right->keys.assign(node->keys.begin() + static_cast<std::ptrdiff_t>(split_idx),
                       node->keys.end());
    right->values.assign(
        node->values.begin() + static_cast<std::ptrdiff_t>(split_idx),
        node->values.end());

    node->keys.resize(split_idx);
    node->values.resize(split_idx);

    right->next = node->next;
    node->next = right;

    InsertResult result;
    result.split = true;
    result.pivot = right->keys.front();
    result.right = right;
    return result;
  }

  const std::size_t child_idx = childIndex(node->keys, key);
  InsertResult child_result =
      insertRecursive(node->children[child_idx], key, value, inserted_new);

  if (!child_result.split) {
    return {};
  }

  node->keys.insert(node->keys.begin() + static_cast<std::ptrdiff_t>(child_idx),
                    child_result.pivot);
  node->children.insert(
      node->children.begin() + static_cast<std::ptrdiff_t>(child_idx + 1),
      child_result.right);

  if (node->keys.size() <= kMaxKeys) {
    return {};
  }

  const std::size_t mid = node->keys.size() / 2;
  const Key pivot = node->keys[mid];

  auto right = std::make_shared<Node>(false);
  right->keys.assign(node->keys.begin() + static_cast<std::ptrdiff_t>(mid + 1),
                     node->keys.end());
  right->children.assign(
      node->children.begin() + static_cast<std::ptrdiff_t>(mid + 1),
      node->children.end());

  node->keys.resize(mid);
  node->children.resize(mid + 1);

  InsertResult result;
  result.split = true;
  result.pivot = pivot;
  result.right = right;
  return result;
}

void BPlusTree::upsert(Key key, const std::string& value) {
  bool inserted_new = false;
  InsertResult result = insertRecursive(root_, key, value, inserted_new);

  if (result.split) {
    auto new_root = std::make_shared<Node>(false);
    new_root->keys.push_back(result.pivot);
    new_root->children.push_back(root_);
    new_root->children.push_back(result.right);
    root_ = new_root;
  }

  if (inserted_new) {
    ++size_;
  }
}

std::shared_ptr<BPlusTree::Node> BPlusTree::leftMostLeaf() const {
  auto node = root_;
  while (node != nullptr && !node->is_leaf) {
    node = node->children.front();
  }
  return node;
}

std::vector<Row> BPlusTree::scan() const {
  std::vector<Row> rows;
  rows.reserve(size_);

  auto node = leftMostLeaf();
  while (node != nullptr) {
    for (std::size_t i = 0; i < node->keys.size(); ++i) {
      rows.push_back(Row{node->keys[i], node->values[i]});
    }
    node = node->next;
  }

  return rows;
}

std::size_t BPlusTree::size() const { return size_; }

void BPlusTree::clear() {
  root_ = std::make_shared<Node>(true);
  size_ = 0;
}

}  // namespace minidb
