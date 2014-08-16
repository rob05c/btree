#include "btree.h"
#include <memory>
#include <cmath>
#include <string>
#include <iostream>

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
  if(size > 0) {
    for(size_t i = size; i > pos; --i) {
      keys[i] = keys[i - 1];
      children[i+1] = move(children[i]);
    }
  }
  keys[pos] = key;
  ++size;
}

void node::insert(key_t key) {
  if(key == 547)
    key = key; // debug
  if(!is_leaf()) {
    const size_t key_pos = find_pos(key);
    if(!children[key_pos])
      children[key_pos] = unique_ptr<node>(new node(this, tree));
    children[key_pos]->insert(key);
    return;
  }

  if(size < range_max) {
    place_key(key, find_pos(key));
    return;
  }

//  node* new_node = 
  unique_ptr<node> empty;
  split(key, empty);
/*
  if(key > keys[size - 1])
     new_node->place_key(key, new_node->find_pos(key));
  else
    place_key(key, find_pos(key));

  if(key > keys[size - 1] && key < new_node->keys[0])
    key = key; // debug
*/
}

/// make a new root, if this node is the root
/// @return the newly created sibling to this node
node* node::reroot() {
  /*
  cout << "rerooting " << key << " keys [";
  for(size_t i = 0, end = size; i != end; ++i)
    cout << keys[i] << ", ";
  cout << "]" << endl;
  cout << "otherkeys [";
  for(size_t i = 0, end = other->size; i != end; ++i)
    cout << other->keys[i] << ", ";
  cout << "]" << endl;
  */

  auto new_root = unique_ptr<node>(new node(nullptr, tree));
  auto new_sibling = unique_ptr<node>(new node(new_root.get(), tree));
  parent = new_root.get();
  
  const size_t median = range_max / 2;
  const key_t median_key = keys[median];
  for(int i = range_max - 1, new_sibling_i = median - 1, end = median; i != end; --i, --new_sibling_i) {
    new_sibling->keys[new_sibling_i] = keys[i];
    new_sibling->children[i + 1] = move(children[i + 1]);
    ++new_sibling->size;
    --size;
  }
  new_sibling->children[0] = move(children[median]);

  new_root->keys[0] = median_key;
  new_root->children[0] = move(tree->root); // tree->root == this
  new_root->children[1] = move(new_sibling);
  new_root->size = 1;

  tree->root = move(new_root);
}

// splits, adds the new node to the parent, and returns it
// new node is on the right/greater, existing is left/lesser
node* node::split(key_t key, unique_ptr<node>& other) {
  cout << "splitting " << key << " min " << keys[0] << " max " << keys[size - 1] << " size " << size << endl;


  unique_ptr<node> new_node(new node(parent, tree));
  node* new_node_raw = new_node.get();

  key_t all_keys[range_max + 1];
  std::unique_ptr<node> all_children[range_max + 2];
  {
    // this could be done more efficiently. IMO it's easier to read this way
    size_t i = 0;
    all_children[0] = move(children[0]);
    for(; i < range_max && keys[i] < key; ++i) {
      all_keys[i] = keys[i];
      all_children[i + 1] = move(children[i + 1]);
    }
    all_keys[i] = key;
    all_children[i + 1] = move(other);
    for(; i < range_max; ++i) {
      all_keys[i + 1] = keys[i];
      all_children[i + 2] = move(children[i + 1]);
    }
    size = 0;
    new_node->size = 0;
  }

  const size_t median = range_max / 2 + 1;
  const key_t median_key = all_keys[median];

  cout << "median chosen " << median << " key " << median_key << " of [";
  for(size_t i = 0, end = range_max + 1; i != end; ++i)
    cout << all_keys[i] << ", ";
  cout << "]" << endl;

  children[0] = move(all_children[0]);
  for(size_t i = 0, end = median; i != end; ++i) {
    keys[i] = all_keys[i];
    children[i + 1] = move(all_children[i + 1]);
    ++size;
  }
  new_node->children[0]  = move(all_children[median + 1]);
  for(size_t i = median + 1, new_node_i = 0, end = range_max + 1; i != end; ++i, ++new_node_i) {
    new_node->keys[new_node_i] = all_keys[i];
    new_node->children[new_node_i + 1] = move(all_children[i + 1]);
    ++new_node->size;
  }

  if(parent == nullptr) {
    auto new_root = unique_ptr<node>(new node(nullptr, tree));
    new_root->children[0] = move(tree->root); // tree->root == this
    tree->root = move(new_root);    
    parent = tree->root.get();
  }

  parent->add(median_key, new_node);
  return new_node_raw;
}

void node::add(key_t key, unique_ptr<node>& other) {
  cout << "adding " << key << " min " << keys[0] << " max " << keys[size - 1] << " size " << size << endl;
  if(size < range_max) {
    const auto pos = find_pos(key);
    place_key(key, pos);
    children[pos + 1] = move(other);
    return;
  }

//  node* new_node = 
  split(key, other);
/*
  if(new_node->keys[0] <= key)
    new_node->add(key, other);
  else
    add(key, other);
 
  if(keys[size - 1] < key && new_node->keys[0] > key)
    key = key;
*/
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
