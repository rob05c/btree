#include "btree.h"
#include <string>
#include <iostream>
#include <cstdlib>

namespace {
using std::cout;
using std::string;

const string default_app_name = "btree";

struct app_arguments {
  bool success;
  string app_name;
  size_t array_size;
};

app_arguments parseArgs(const int& argc, const char** argv) {
  app_arguments args;
  args.success = false;

  if(argc < 1)
    return args;
  args.app_name = argv[0];

  if(argc < 2)
    return args;
  args.array_size = strtol(argv[1], NULL, 10);

  args.success = true;
  return args;
}

string usage_message(const string& app_name) {
  return "usage: " + (app_name.empty() ? default_app_name : app_name) + " size" + "\n";
}

/*
ord_t random_ord(ord_t min, ord_t max) {
  const auto rand_ord = (ord_t) rand() / RAND_MAX;
  return min + rand_ord * (max - min);
 }
*/

} // namespace

int main(int argc, char* argv[]) {
  srand(time(NULL));
//  srand(42); // set a deterministic seed

/*
  app_arguments args = parseArgs(argc, (const char**)argv);
  if(!args.success) {
    cout << usage_message(args.app_name);
    return 0;
  }
*/

  btree::tree tree;

  const auto NUM = 1000u;
  const auto RANGE = 1000u;
  for(size_t i = NUM, end = 0; i != end; --i) {
    const key_t key = rand() % RANGE;
//    const key_t key = i;
    tree.insert(key);
  }

  cout << tree.str() << std::endl;

  return 0;
}
