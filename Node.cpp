#include "Node.h"

Node *Node::most_left_in_subtree(Node *root)
{
    Node *v = root;
    while (v->l)
    {
        v = v->l;
    }
    return v;
}

Node *Node::next_in_update_order(Node *v)
{
    if (!v->a)
    {
        return nullptr;
    }
    if (v == v->a->l)
    {
        if (v->a->r)
        {
            return most_left_in_subtree(v->a->r);
        }
        return v->a;
    }
    return v->a;
}