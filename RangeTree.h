
#ifndef CACHEOBLIVIOUSRANGETREE_RANGETREE_H
#define CACHEOBLIVIOUSRANGETREE_RANGETREE_H

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
};

#endif //CACHEOBLIVIOUSRANGETREE_RANGETREE_H
