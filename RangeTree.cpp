#include <functional>
#include <queue>
#include <stack>
#include <algorithm>

#include <glog/logging.h>
#include "RangeTree.h"

//////////////////////////////////////////////////////////////////////////
// Aggregate function
//////////////////////////////////////////////////////////////////////////

std::function<int(int, int)> aggregate_func = [](int a, int b)
{ return a + b; };

int aggregate_func_neutral = 0;

int aggregate_existing_children(Node *v)
{
    if (!(v->l) && !(v->r))
    {
        return v->x;
    }
    int x = aggregate_func_neutral;

    if (v->l)
    {
        x = aggregate_func(x, v->l->x);
    }
    if (v->r)
    {
        x = aggregate_func(x, v->r->x);
    }
    return x;
}

//////////////////////////////////////////////////////////////////////////
// Auxiliary functions
//////////////////////////////////////////////////////////////////////////

void update_full_tree(Node *root)
{
    Node *v = Node::most_left_in_subtree(root);
    while (v)
    {
        v->x = aggregate_existing_children(v);
        v = Node::next_in_update_order(v);
    }
}

std::vector<Node *> add_deep_children(Node *root, int root_h)
{
    std::vector<Node *> deep_children;

    std::queue<std::pair<Node *, int>> q;
    q.push(std::make_pair(root, root_h));
    while (!q.empty())
    {
        Node *v = q.front().first;
        int h = q.front().second;
        q.pop();
        if (h == 0)
        {
            deep_children.push_back(v);
            continue;
        }
        if (v->l)
        {
            q.push(std::make_pair(v->l, h - 1));
        }
        if (v->r)
        {
            q.push(std::make_pair(v->r, h - 1));
        }
    }
    return deep_children;
}

void fillvEBIndexes(Node *root, int root_h)
{
    int last_index = 0;

    std::stack<std::pair<Node *, int>> st;
    st.push(std::make_pair(root, root_h));
    while (!st.empty())
    {
        Node *v = st.top().first;
        int h = st.top().second;
        st.pop();
        if (h == 1)
        {
            v->i_vEB = last_index++;
            continue;
        }
        // cause of stack all will be in reversed order
        auto deep_children = add_deep_children(v, h / 2);
        std::reverse(deep_children.begin(), deep_children.end());
        for (auto child : deep_children)
        {
            st.push(std::make_pair(child, h / 2 + h % 2));
        }
        st.push(std::make_pair(v, h / 2));
    }
}

//////////////////////////////////////////////////////////////////////////
// Interface
// Tree Building
//////////////////////////////////////////////////////////////////////////

std::vector<Node> RangeTree::makeDefaultTree(int N_elements, std::vector<int> &underlied_array)
{
    std::vector<Node> nodes;
    // we need no reallocation!
    nodes.reserve(4 * N_elements);
    std::queue<int> q;
    nodes.push_back(Node());
    nodes.back().left_border = 1;
    nodes.back().right_border = N_elements;
    nodes.back().trivial_i = 0;
    q.push(0);
    while (!q.empty())
    {
        int current_v = q.front();
        q.pop();
        auto &v = nodes[current_v];

        int middle = v.left_border + (v.right_border - v.left_border) / 2;

        if (v.left_border == v.right_border)
        {
            v.x = underlied_array[v.left_border - 1];
            continue;
        }
//        if (v.left_border != middle_border_left)
        {
            nodes.push_back(Node());
            v.l = &nodes.back();
            v.l->left_border = v.left_border;
            v.l->right_border = middle;
            v.l->a = &nodes[current_v];
            v.l->trivial_i = (int) nodes.size() - 1;
            q.push(v.l->trivial_i);
        }
//        if (middle_border_right != v.right_border)
        {
            nodes.push_back(Node());
            v.r = &nodes.back();
            v.r->left_border = middle + 1;
            v.r->right_border = v.right_border;
            v.r->a = &nodes[current_v];
            v.r->trivial_i = (int) nodes.size() - 1;
            q.push(v.r->trivial_i);
        }
    }
    update_full_tree(&nodes[0]);
    return nodes;
}

std::vector<Node> RangeTree::make_vEB_tree(int N_elements, std::vector<Node> &nodes)
{
    std::vector<Node> nodes_vEB(nodes.capacity());
    fillvEBIndexes(&nodes[0], (int) ceil(log2(N_elements) + 1));

    std::vector<Node *> filtered_nodes;
    for (Node &v : nodes)
    {
        filtered_nodes.push_back(&v);
    }
    std::sort(filtered_nodes.begin(), filtered_nodes.end(), [](Node *x, Node *y)
    { return x->i_vEB < y->i_vEB; });

    for (auto vp : filtered_nodes)
    {
        nodes_vEB[vp->i_vEB] = Node(*vp);
        DCHECK(vp->i_vEB < nodes_vEB.size());

        DCHECK(!(vp->l) || (vp->l->i_vEB != -1));
        DCHECK(!(vp->r) || (vp->r->i_vEB != -1));
        DCHECK(!(vp->a) || (vp->a->i_vEB != -1));
        nodes_vEB[vp->i_vEB].l = (vp->l) ? &nodes_vEB[vp->l->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].r = (vp->r) ? &nodes_vEB[vp->r->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].a = (vp->a) ? &nodes_vEB[vp->a->i_vEB] : nullptr;
    }
    return nodes_vEB;
}

//////////////////////////////////////////////////////////////////////////
// Interface
// Queries
//////////////////////////////////////////////////////////////////////////

Node *RangeTree::find(Node *root, int pos)
{
    Node *v = root;

    while (true)
    {
        if (v->left_border == v->right_border)
        {
            return v;
        }
        if (v->l && v->l->right_border >= pos)
        {
            v = v->l;
            continue;
        }
        if (v->r && v->r->left_border <= pos)
        {
            v = v->r;
            continue;
        }
        // unsuitable situation
        DCHECK(false);
    }
}

void RangeTree::update(Node *root, int pos, int new_val)
{
    Node *v = find(root, pos);
    DCHECK(v);
    v->x = new_val;
    v = v->a;
    while (v)
    {
        v->x = aggregate_existing_children(v);
        v = v->a;
    }
}

int RangeTree::query(Node *v, int left, int right)
{
    if (left > right)
    {
        return aggregate_func_neutral;
    }
    if (left == v->left_border && right == v->right_border)
    {
        return v->x;
    }
    int x = aggregate_func_neutral;
    if (v->l)
    {
        x = aggregate_func(x, query(v->l, left, std::min(right, v->l->right_border)));
    }
    if (v->r)
    {
        x = aggregate_func(x, query(v->r, std::max(left, v->r->left_border), right));
    }
    return x;
}
