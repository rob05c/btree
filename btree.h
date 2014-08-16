#ifndef btreeH
#define btreeH

#include <vector>
#include <memory>
#include <limits>

namespace btree {

// 2-5 tree
const size_t range_min = 2;
const size_t range_max = 5;

typedef int key_t;

class tree;

class node {
public:
  node(node* parent_, tree* tree_) : size(0), parent(parent_), tree(tree_) {}

  void        insert(key_t key);
  std::string str();

  void check_invariants(); // debug
private:
  static void split(node* old_node);

  void   add(key_t key, std::unique_ptr<node>& other);
  bool   is_leaf();
  size_t find_pos(const key_t& key);
  void   place_key(const key_t& key, const size_t pos);

  btree::tree* tree;
  node*        parent;
  size_t                size;
  key_t                 keys[range_max];
  std::unique_ptr<node> children[range_max + 1];

  friend class tree; // tree::reroot needs access to parent, split.
};

class tree {
public:
  tree() : root(std::unique_ptr<node>(new node(nullptr, this))) {}
  void        insert(key_t key);
  std::string str();
private:
  void reroot();

  std::unique_ptr<node> root;
};

} // namespace rtree

#endif
