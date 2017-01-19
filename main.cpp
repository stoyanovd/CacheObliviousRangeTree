#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>

#include "RangeTree.h"
#include "TreePrinter.h"

std::vector<Node> makeDefaultTree(int N_elements)
{
    // TODO is it enough 2N
//    std::vector<Node> nodes(2 * N_elements + 1, Node());
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
    return nodes;
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

//const int N = 10000;
//const int N = 70;
//const int N = 65;
//const int N = 31;

int main()
{
    std::cout << "Node size: " << sizeof(Node) << " bytes." << std::endl;
    std::cout << "Cache size (on my processor): 6144 KB." << std::endl;
    const int N_in_block = 6144 * 1024 / sizeof(Node);
    std::cout << "  So, nearly " << N_in_block << " nodes will be in one cache block." << std::endl;
    const int BLOCKS_NUMBER = 500;
    std::cout << "Let's say, we want to work with " << BLOCKS_NUMBER << " blocks." << std::endl;
    int N = N_in_block * BLOCKS_NUMBER;
    std::cout << "So, we need " << N << " nodes." << std::endl;
    std::cout << "To be definite, there will be " << N << " elements." << std::endl;
    std::cout << "And nearly " << 2 * N << " nodes accordingly." << std::endl;

    N = 30;
    std::vector<Node> nodes = makeDefaultTree(N);
    if (N < 140)
    {
        print_tree_to_png(&nodes[0], "tree_trivial", [](Node *v)
        { return v->trivial_i; });
    }


    nodes.erase(
        std::remove_if(nodes.begin(), nodes.end(), [](Node &v)
        { return v.trivial_i == -1; }),
        nodes.end()
    );

    fillvEBIndexes(&nodes[0], (int) ceil(log2(N) + 1));

    // let's make firstly
    // afterwards will make it more clean

    std::vector<Node *> filtered_nodes;
    for (Node &v : nodes)
    {
        filtered_nodes.push_back(&v);
    }
    // TODO Why commenting this cause free(): invalid pointer
//    filtered_nodes.erase(
//        std::remove_if(filtered_nodes.begin(), filtered_nodes.end(), [](Node *v)
//        { return v->trivial_i == -1 || v->i_vEB == -1; }),
//        filtered_nodes.end()
//    );
    std::sort(filtered_nodes.begin(), filtered_nodes.end(), [](Node *x, Node *y)
    { return x->i_vEB < y->i_vEB; });

    std::vector<Node> nodes_vEB(4 * N);
    for (auto vp : filtered_nodes)
    {
        nodes_vEB[vp->i_vEB] = Node(*vp);
        assert(vp->i_vEB < 4 * N);

        // TODO make nodes large enough

        assert(!(vp->l) || (vp->l->i_vEB != -1));
        assert(!(vp->r) || (vp->r->i_vEB != -1));
        assert(!(vp->a) || (vp->a->i_vEB != -1));
        nodes_vEB[vp->i_vEB].l = (vp->l && vp->l->i_vEB != -1) ? &nodes_vEB[vp->l->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].r = (vp->r && vp->r->i_vEB != -1) ? &nodes_vEB[vp->r->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].a = (vp->a && vp->a->i_vEB != -1) ? &nodes_vEB[vp->a->i_vEB] : nullptr;
    }

    if (N < 140)
    {
        print_tree_to_png(&nodes_vEB[0], "tree_vEB", [](Node *v)
        { return v->i_vEB; });
    }
    std::cout << "Finish." << std::endl;
    return 0;
}