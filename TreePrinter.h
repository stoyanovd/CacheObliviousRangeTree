#ifndef CACHEOBLIVIOUSRANGETREE_TREEPRINTER_H
#define CACHEOBLIVIOUSRANGETREE_TREEPRINTER_H

#include <cstdio>
#include "RangeTree.h"
#include <functional>


void bst_print_dot(Node *tree, FILE *stream, std::function<int(Node *)> &node_printer);

void print_tree_to_png(Node *tree, const std::string &filename, std::function<int(Node *)> &&node_printer);

#endif //CACHEOBLIVIOUSRANGETREE_TREEPRINTER_H
