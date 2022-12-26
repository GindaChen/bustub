//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// p0_trie.h
//
// Identification: src/include/primer/p0_trie.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "common/rwlatch.h"

namespace bustub {

/**
 * TrieNode is a generic container for any node in Trie.
 */
class TrieNode {
 public:
  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Construct a new Trie Node object with the given key char.
   * is_end_ flag should be initialized to false in this constructor.
   *
   * @param key_char Key character of this trie node
   */
  explicit TrieNode(char key_char) {
    this->is_end_ = false;
    this->key_char_ = key_char;
  }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Move constructor for trie node object. The unique pointers stored
   * in children_ should be moved from other_trie_node to new trie node.
   *
   * @param other_trie_node Old trie node.
   */
  TrieNode(TrieNode &&other_trie_node) noexcept {
    this->is_end_ = other_trie_node.is_end_;
    this->key_char_ = other_trie_node.key_char_;
    // Assign unique pointer, then remove the old elements.
    auto &a = this->children_;
    auto &b = other_trie_node.children_;
    b.insert(std::make_move_iterator(a.begin()), std::make_move_iterator(a.end()));
    a.erase(a.begin(), a.end());
  }

  /**
   * @brief Destroy the TrieNode object.
   */
  virtual ~TrieNode() = default;

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Whether this trie node has a child node with specified key char.
   *
   * @param key_char Key char of child node.
   * @return True if this trie node has a child with given key, false otherwise.
   */
  [[nodiscard]] bool HasChild(char key_char) const {
    const auto &map = this->children_;
    return map.find(key_char) != map.cend();
  }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Whether this trie node has any children at all. This is useful
   * when implementing 'Remove' functionality.
   *
   * @return True if this trie node has any child node, false if it has no child node.
   */
  [[nodiscard]] bool HasChildren() const { return !this->children_.empty(); }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Whether this trie node is the ending character of a key string.
   *
   * @return True if is_end_ flag is true, false if is_end_ is false.
   */
  [[nodiscard]] bool IsEndNode() const { return this->is_end_; }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Return key char of this trie node.
   *
   * @return key_char_ of this trie node.
   */
  [[nodiscard]] char GetKeyChar() const { return this->key_char_; }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Insert a child node for this trie node into children_ map, given the key char and
   * unique_ptr of the child node. If specified key_char already exists in children_,
   * return nullptr. If parameter `child`'s key char is different than parameter
   * `key_char`, return nullptr.
   *
   * Note that parameter `child` is rvalue and should be moved when it is
   * inserted into children_map.
   *
   * The return value is a pointer to unique_ptr because pointer to unique_ptr can access the
   * underlying data without taking ownership of the unique_ptr. Further, we can set the return
   * value to nullptr when error occurs.
   *
   * @param key Key of child node
   * @param child Unique pointer created for the child node. This should be added to children_ map.
   * @return Pointer to unique_ptr of the inserted child node. If insertion fails, return nullptr.
   */
  std::unique_ptr<TrieNode> *InsertChildNode(char key_char, std::unique_ptr<TrieNode> &&child) {
    if (this->HasChild(key_char)) {
      return nullptr;
    }
    if (key_char != child->key_char_){
      return nullptr;
    }
    this->children_[key_char] = std::move(child);
    auto result = &this->children_[key_char];
    return result;
  }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Get the child node given its key char. If child node for given key char does
   * not exist, return nullptr.
   *
   * @param key Key of child node
   * @return Pointer to unique_ptr of the child node, nullptr if child
   *         node does not exist.
   */
  std::unique_ptr<TrieNode> *GetChildNode(char key_char) {
    if (!this->HasChild(key_char)) {
      return nullptr;
    }
    auto result = &this->children_[key_char];
    return result;
  }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Remove child node from children_ map.
   * If key_char does not exist in children_, return immediately.
   *
   * @param key_char Key char of child node to be removed
   */
  void RemoveChildNode(char key_char) { this->children_.erase(key_char); }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Set the is_end_ flag to true or false.
   *
   * @param is_end Whether this trie node is ending char of a key string
   */
  void SetEndNode(bool is_end) { this->is_end_ = is_end; }

  /**
   * @brief Return a const reference to children.
   *
   * @return Const reference to the map of children.
   */
  [[nodiscard]] const std::unordered_map<char, std::unique_ptr<TrieNode>> &GetChildren() const {
    return this->children_;
  }

 protected:
  /** Key character of this trie node */
  char key_char_;
  /** whether this node marks the end of a key */
  bool is_end_{false};
  /** A map of all child nodes of this trie node, which can be accessed by each
   * child node's key char. */
  std::unordered_map<char, std::unique_ptr<TrieNode>> children_;
};

/**
 * TrieNodeWithValue is a node that marks the ending of a key, and it can
 * hold a value of any type T.
 */
template <typename T>
class TrieNodeWithValue : public TrieNode {
 private:
  /* Value held by this trie node. */
  T value_;

 public:
  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Construct a new TrieNodeWithValue object from a TrieNode object and specify its value.
   * This is used when a non-terminal TrieNode is converted to terminal TrieNodeWithValue.
   *
   * The children_ map of TrieNode should be moved to the new TrieNodeWithValue object.
   * Since it contains unique pointers, the first parameter is a rvalue reference.
   *
   * You should:
   * 1) invoke TrieNode's move constructor to move data from TrieNode to
   * TrieNodeWithValue.
   * 2) set value_ member variable of this node to parameter `value`.
   * 3) set is_end_ to true
   *
   * @param trieNode TrieNode whose data is to be moved to TrieNodeWithValue
   * @param value
   */
  TrieNodeWithValue(TrieNode &&trieNode, T value) : TrieNode(std::move(trieNode)) {
    this->value_ = value;
    this->SetEndNode(true);
  }

  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Construct a new TrieNodeWithValue. This is used when a new terminal node is constructed.
   *
   * You should:
   * 1) Invoke the constructor for TrieNode with the given key_char.
   * 2) Set value_ for this node.
   * 3) set is_end_ to true.
   *
   * @param key_char Key char of this node
   * @param value Value of this node
   */
  TrieNodeWithValue(char key_char, T value) : TrieNode(key_char) {
    this->value_ = value;
    this->SetEndNode(true);
  }

  /**
   * @brief Destroy the Trie Node With Value object
   */
  ~TrieNodeWithValue() override = default;

  /**
   * @brief Get the stored value_.
   *
   * @return Value of type T stored in this node
   */
  T GetValue() const { return value_; }
};

/**
 * Trie is a concurrent key-value store. Each key is a string and its corresponding
 * value can be any type.
 */
class Trie {
 private:
  /* Root node of the trie */
  std::unique_ptr<TrieNode> root_;
  /* Read-write lock for the trie */
  // ReaderWriterLatch latch_;

 public:
  /**
   * TODO(P0): Add implementation <done>
   *
   * @brief Construct a new Trie object. Initialize the root node with '\0'
   * character.
   */
  Trie() {
    root_ = std::make_unique<TrieNode>(TrieNode('\0'));
    //    root_ = std::unique_ptr(new TrieNode('\0')); // Strangely, template deduction did not trigger...
  }

  /**
   * TODO(P0): Add implementation <done, but need check>
   *
   * @brief Insert key-value pair into the trie.
   *
   * If the key is an empty string, return false immediately.
   *
   * If the key already exists, return false. Duplicated keys are not allowed and
   * you should never overwrite value of an existing key.
   *
   * When you reach the ending character of a key:
   * 1. If TrieNode with this ending character does not exist, create new TrieNodeWithValue
   * and add it to parent node's children_ map.
   * 2. If the terminal node is a TrieNode, then convert it into TrieNodeWithValue by
   * invoking the appropriate constructor.
   * 3. If it is already a TrieNodeWithValue,
   * then insertion fails and returns false. Do not overwrite existing data with new data.
   *
   * You can quickly check whether a TrieNode pointer holds TrieNode or TrieNodeWithValue
   * by checking the is_end_ flag. If is_end_ == false, then it points to TrieNode. If
   * is_end_ == true, it points to TrieNodeWithValue.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @param value Value to be inserted
   * @return True if insertion succeeds, false if the key already exists
   */
  template <typename T>
  bool Insert(const std::string &key, T value) {
    if (key.empty()) {
      return false;
    }

    auto node = &root_;
    auto parent = &root_;

    char last_key = 0;
    for (auto k : key) {
      last_key = k;
      if ((*node)->HasChild(k)) {
        parent = node;
        node = (*node)->GetChildNode(k);
        continue;
      }
      // Create an internal node, and swap
      // TODO: (P0:Style) How to properly access value of pointer to unique_ptr?
      (*node)->InsertChildNode(k, std::make_unique<TrieNode>(k));
      parent = node;
      node = (*node)->GetChildNode(k);
    }

    assert(node != nullptr);
    assert(parent != nullptr);
    assert(parent != node);

    if ((*node)->IsEndNode()) {
      // Encountered a TrieNodeWithValue - (3)
      return false;
    }

    // Encountered a TrieNode - (1), (2). Anyways, convert it to TrieNodeWithValue
    // auto q = std::unique_ptr<TrieNodeWithValue<T>>(
    //     new TrieNodeWithValue(std::move((*(*node))), value)
    // );

    auto q = std::make_unique<TrieNodeWithValue<T>>(std::move(**node), value);

    // Not last key you are supposed to remove - it's the previous key.
    (*parent)->RemoveChildNode(last_key);
    (*parent)->InsertChildNode(last_key, std::move(q));

    return true;
  }

  /**
   * TODO(P0): Add implementation <done, but need check>
   *
   * @brief Remove key value pair from the trie.
   * This function should also remove nodes that are no longer part of another
   * key. If key is empty or not found, return false.
   *
   * You should:
   * 1) Find the terminal node for the given key.
   * 2) If this terminal node does not have any children, remove it from its
   * parent's children_ map.
   * 3) Recursively remove nodes that have no children and are not terminal node
   * of another key.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @return True if the key exists and is removed, false otherwise
   */
  bool Remove(const std::string &key) {
    if (key.empty()) {
      return false;
    }

    auto node = &root_;

    // Track traversal chain. Use for recursive backtrack deletion.
    std::vector<std::pair<std::unique_ptr<TrieNode> *, char>> stack;

    char last_key = 0;
    for (auto k : key) {
      stack.emplace_back(std::make_pair(node, k));
      if (root_->HasChild(k)) {
        node = (*node)->GetChildNode(k);
        continue;
      }
      // Key not found - early return.
      return false;
    }

    assert(node != nullptr);
    assert(last_key != 0);

    // Key not terminal node - early return.
    if (!(*node)->IsEndNode()) {
      return false;
    }

    assert(!stack.empty());

    // Reset the child node to a TrieNode.
    // (used inner scope to avoid name collision / shadowing)
    {
      auto [parent, k] = stack.back();
      stack.pop_back();
      auto child = (*parent)->GetChildNode(k);
      auto new_child = std::make_unique<TrieNode>(TrieNode(std::move(**child)));
      (*parent)->RemoveChildNode(k);
      (*parent)->InsertChildNode(k, std::move(new_child));
    }

    // Delete node unused by other charts.
    while (!stack.empty()) {
      auto [parent, k] = stack.back();
      auto child = (*parent)->GetChildNode(k);
      if ((*child)->HasChildren()) {
        // Early stop if found children within the node.
        break;
      }
      // Otherwise, empty out this node from parent.
      parent->get()->RemoveChildNode(k);
      stack.pop_back();
    }

    return true;
  }

  /**
   * TODO(P0): Add implementation <done, but need check>
   *
   * @brief Get the corresponding value of type T given its key.
   * If key is empty, set success to false.
   * If key does not exist in trie, set success to false.
   * If the given type T is not the same as the value type stored in TrieNodeWithValue
   * (ie. GetValue<int> is called but terminal node holds std::string),
   * set success to false.
   *
   * To check whether the two types are the same, dynamic_cast
   * the terminal TrieNode to TrieNodeWithValue<T>. If the casted result
   * is not nullptr, then type T is the correct type.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @param success Whether GetValue is successful or not
   * @return Value of type T if type matches
   */
  template <typename T>
  T GetValue(const std::string &key, bool *success) {
    *success = false;
    // Key empty
    if (key.empty()) {
      return {};
    }

    // Try to find the key alone the trie chain.
    auto node = &this->root_;
    for (auto k : key) {
      node = (*node)->GetChildNode(k);
      if (node == nullptr) {
        return {};
      }
    }

    // Found the node. Now, can we take the value?
    assert(node != nullptr);
    if (!(*node)->IsEndNode()) {
      // Nope since it's not a terminal node.
      return {};
    }

    // Definitely can, since it is indeed a terminal node.
    auto p_maybe_node_with_value = dynamic_cast<TrieNodeWithValue<T> *>(node->get());
    if (p_maybe_node_with_value != nullptr) {
      return p_maybe_node_with_value->GetValue();
    }
    throw std::runtime_error(
        "Encountered TrieNode with IsEndNode() == true. "
        "Is the dynamic_cast problematic, "
        "or our setting has problem? ");
  }

  /**
   * @brief Return the const reference to the root.
   * @return Return the const reference to the root.
   */
  [[nodiscard]] const std::unique_ptr<TrieNode> &GetRoot() const { return this->root_; }
};
}  // namespace bustub
