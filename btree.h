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

struct node {
  node(node* parent_, tree* tree_) : size(0), parent(parent_), tree(tree_) {}
  void insert(key_t key);
  std::string str();
  bool is_leaf();
  size_t find_pos(const key_t& key);
  void place_key(const key_t& key, const size_t pos);
  node* split(key_t key, std::unique_ptr<node>& other);
  node* reroot();
  void add(key_t key, std::unique_ptr<node>& other);

  size_t size;
  key_t keys[range_max];
  std::unique_ptr<node> children[range_max + 1];
  node* parent;
  btree::tree* tree;
};

class tree {
public:
  tree() : root(std::unique_ptr<node>(new node(nullptr, this))) {}
  void insert(key_t key) {root->insert(key);}
  std::string str();

  std::unique_ptr<node> root;
};

} // namespace rtree

#endif
