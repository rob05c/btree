#include "btree.h"
#include <memory>
#include <cmath>
#include <string>
#include <iostream>
#include <cassert>

namespace {
using std::unique_ptr;
using std::move;
using std::vector;
using std::ceil;
using std::string;
using std::to_string;
using std::cout;
using std::endl;
}

namespace btree {
void node::check_invariants() {
#ifdef NDEBUG
  return
#else
  for(auto i = size + 1; i != range_max + 1; ++i) {
    if(children[i].get() != nullptr) {
      fprintf(stderr, "node found with child past size. Debugging suggested.");
    }
  }
#endif
}

bool node::is_leaf() {
  for(size_t i = 0, end = size; i != end; ++i) {
    if((bool) children[i])
      return false;
  }
  return true;
}

size_t node::find_pos(const key_t& key) {
  size_t pos = 0;
  for(size_t end = size; pos != end && keys[pos] < key; ++pos);
  return pos;
}

void node::place_key(const key_t& key, const size_t pos) {
  check_invariants();

  if(size > 0) {
    for(size_t i = size; i > pos; --i) {
      keys[i] = keys[i - 1];
      if((bool) children[i+1])
        size = size; // debug
      children[i+1] = move(children[i]);
    }
  }
  keys[pos] = key;
  ++size;

  check_invariants();
}

void node::insert(key_t key) {
  check_invariants();


  if(!is_leaf()) {
    auto key_pos = find_pos(key);
    if(!children[key_pos])
      children[key_pos] = unique_ptr<node>(new node(this));
    else if(children[key_pos]->size == range_max) {
      split(children[key_pos].get());
      key_pos = find_pos(key);
    }
    children[key_pos]->insert(key);

    check_invariants();
    return;
  }

  place_key(key, find_pos(key));

  check_invariants();
}

void node::add(key_t key, unique_ptr<node>& other) {
  check_invariants();
  assert(size < range_max);
  assert(other.get() != nullptr);

  const auto pos = find_pos(key);
  place_key(key, pos);
  children[pos + 1] = move(other);
  children[pos + 1]->parent = this;

  check_invariants();
}

/// @todo make member of node. static??
void node::split(node* old_node) {
  old_node->check_invariants();
  old_node->parent->check_invariants();

  assert(old_node);
  assert(old_node->parent);
  assert(old_node->parent->size < range_max);

  unique_ptr<node> new_node(new node(old_node->parent));
  node* new_node_raw = new_node.get();

  const size_t median = range_max / 2;
  const key_t median_key = old_node->keys[median];

  if((bool) old_node->children[median + 1]) {
    new_node->children[0] = move(old_node->children[median + 1]);
    new_node->children[0]->parent = new_node_raw;
  }
  for(size_t i = median + 1, new_node_i = 0, end = range_max; i != end; ++i, ++new_node_i) {
    new_node->keys[new_node_i] = old_node->keys[i];
    if(old_node->children[i + 1]) {
      new_node->children[new_node_i + 1] = move(old_node->children[i + 1]);
      new_node->children[new_node_i + 1]->parent = new_node.get();
    }
    ++new_node->size;
    --old_node->size;
  }
  --old_node->size; // remove the median

  old_node->parent->add(median_key, new_node);

  old_node->check_invariants();
  new_node_raw->check_invariants();
  old_node->parent->check_invariants();
}

void tree::reroot() {
  assert(root);
  root.get()->check_invariants();

  auto new_root = unique_ptr<node>(new node(nullptr));
  auto old_root_raw = root.get();
  root->parent = new_root.get();
  new_root->children[0] = move(root);
  new_root->children[0]->parent = new_root.get();
  root = move(new_root);
  node::split(old_root_raw);

  root.get()->check_invariants();
  old_root_raw->check_invariants();
}

void tree::insert(key_t key) {
  if(root->size == range_max)
    reroot();
  root->insert(key);
}

string node::str() {
  string s;
//  s += "["; // debug
  for(size_t i = 0, end = size; i != end; ++i) {
    if((bool) children[i])
      s += children[i]->str() + ", ";
    s += to_string(keys[i]) + ", ";
  }

  if((bool) children[size])
    s += children[size]->str();
  else
    s.erase(s.end() -2, s.end()); // erase the last ", "
//  s += "]"; // debug
  return s;
}

string tree::str() {
  return "[" + root->str() + "]";
}

}  // namespace btree
