#ifndef CACHEOBLIVIOUSRANGETREE_NODE_H
#define CACHEOBLIVIOUSRANGETREE_NODE_H

const int PLACE_HOLDER_SIZE = 200;

struct Node
{
    Node *l = nullptr;
    Node *r = nullptr;
    Node *a = nullptr;
    int i_vEB = -1;
    int trivial_i = -1;
    int left_border, right_border;
    int x;

//    int place_holder[PLACE_HOLDER_SIZE];

    static Node *next_in_update_order(Node *v);
    static Node *most_left_in_subtree(Node *root);
};
#endif //CACHEOBLIVIOUSRANGETREE_NODE_H
