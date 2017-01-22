#ifndef CACHEOBLIVIOUSRANGETREE_RANGETREE_H
#define CACHEOBLIVIOUSRANGETREE_RANGETREE_H

#include <vector>
#include "Node.h"

struct RangeTree
{
    // Tree Building
    static std::vector<Node> makeDefaultTree(int N_elements, std::vector<int> &underlied_array);
    static std::vector<Node> make_vEB_tree(int N_elements, std::vector<Node> &nodes);

    // Queries
    static int query(Node *v, int left, int right);
    static void update(Node *root, int pos, int new_val);
    static Node *find(Node *root, int pos);
};

#endif //CACHEOBLIVIOUSRANGETREE_RANGETREE_H
