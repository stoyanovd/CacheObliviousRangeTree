#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <chrono>
#include <fstream>
#include <zconf.h>

#include "RangeTree.h"
#include "TreePrinter.h"

std::vector<Node> makeDefaultTree(int N_elements, std::vector<int> &underlied_array)
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
    return nodes;
}

Node *most_left_in_subtree(Node *root)
{
    Node *v = root;
    while (v->l)
    {
        v = v->l;
    }
    return v;
}
Node *next_in_update_order(Node *v)
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
void update_full_tree(Node *root)
{
    Node *v = most_left_in_subtree(root);
    while (v)
    {
        v->x = aggregate_existing_children(v);
        v = next_in_update_order(v);
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

Node *find_node(Node *root, int pos)
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
        assert(false);
    }
}

void update(Node *root, int pos, int new_val)
{
    Node *v = find_node(root, pos);
    assert(v);
    v->x = new_val;
    v = v->a;
    while (v)
    {
        v->x = aggregate_existing_children(v);
        v = v->a;
    }
}

int query(Node *v, int left, int right)
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

struct Task
{
    bool is_update;
    // union will be suitable...
    int l, r, ans;
    bool ans_is_filled;
    int pos, new_val;
};

void read_input_from_file(std::string const &file,
                          std::vector<int> &array_from_file,
                          std::vector<Task> &tasks_from_file)
{
    std::ifstream ifs;
    ifs.open(file, std::ios_base::in);
    int N_elements = 0;
    ifs >> N_elements;

    for (int i = 0; i < N_elements; i++)
    {
        int x;
        ifs >> x;
        array_from_file.push_back(x);
    }

    int number_tests;
    ifs >> number_tests;

    for (int i = 0; i < number_tests; i++)
    {
        Task task = Task();
        ifs >> task.is_update;
        if (task.is_update)
        {
            ifs >> task.pos >> task.new_val;
        }
        else
        {
            task.ans_is_filled = true;
            ifs >> task.l >> task.r >> task.ans;
        }
        tasks_from_file.push_back(task);
    }
}

void make_random_input(int N_elements, int number_tests,
                       std::vector<int> &array, std::vector<Task> &tasks)
{
    for (int i = 0; i < N_elements; i++)
    {
        array.push_back(rand() % 17239);
    }
    for (int i = 0; i < number_tests; i++)
    {
        Task task = Task();
        task.is_update = (bool) (rand() % 2);
        if (task.is_update)
        {
            task.pos = rand() % N_elements;
            task.new_val = rand() % 17239;
        }
        else
        {
            int x = rand() % N_elements;
            int y = rand() % N_elements;
            task.l = std::min(x, y);
            task.r = std::max(x, y);
            task.ans_is_filled = false;
        }
        tasks.push_back(task);
    }
}

void benchmark_tree(Node *root, std::vector<Task> tasks)
{
    for (auto task : tasks)
    {
        if (task.is_update)
        {
            update(root, task.pos, task.new_val);
        }
        else
        {
            int ans = query(root, task.l, task.r);
            if (task.ans_is_filled)
            {
                if (ans != task.ans)
                {
                    std::cout << "Test with ans failed. l = " << task.l
                              << "; r = " << task.r
                              << "; ans = " << task.ans
                              << "; prog ans = " << ans
                              << std::endl;
                }
                assert(ans == task.ans);
            }
        }
    }
}

std::vector<Node> make_vEB_tree(int N_elements, std::vector<Node> &nodes)
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
        assert(vp->i_vEB < 4 * N_elements);

        assert(!(vp->l) || (vp->l->i_vEB != -1));
        assert(!(vp->r) || (vp->r->i_vEB != -1));
        assert(!(vp->a) || (vp->a->i_vEB != -1));
        nodes_vEB[vp->i_vEB].l = (vp->l) ? &nodes_vEB[vp->l->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].r = (vp->r) ? &nodes_vEB[vp->r->i_vEB] : nullptr;
        nodes_vEB[vp->i_vEB].a = (vp->a) ? &nodes_vEB[vp->a->i_vEB] : nullptr;
    }
    return nodes_vEB;
}

template<class T>
inline T sqr(T x)
{
    return x * x;
}

const int L2_SIZE = 256 * 1024;

const int L3_SIZE = 6144 * 1024;

int clean_cache(int c)
{
    std::vector<int> a(L3_SIZE);
    int s = 0;
    for (int i = 0; i < a.size(); i++)
    {
        a[i] = i * 5 - 2 + c;
    }
    for (int i = 0; i < a.size(); i++)
    {
        s += a[i];
    }
    return s;
}

void clean_cache_L3()
{
    sync();

    std::ofstream ofs("/proc/sys/vm/drop_caches");
    ofs << "3" << std::endl;
}

#define WRITE_PNGS 0
#define BENCHMARK_MODE 1

int main()
{
    const int N_in_block = 6144 * 1024 / sizeof(Node);
//    const int N_in_block = 32 * 1024 / sizeof(Node);
//    const int N_in_block = 256 * 1024 / sizeof(Node);
    const int BLOCKS_NUMBER = 90;
    int N_elements = N_in_block * BLOCKS_NUMBER;
    // N_elements = 30

    std::cout << "Node size: " << sizeof(Node) << " bytes." << std::endl;
    std::cout << "Cache size (on my processor): 6144 KB." << std::endl;
    std::cout << "  So, nearly " << N_in_block << " nodes will be in one cache block." << std::endl;
    std::cout << "Let's say, we want to work with " << BLOCKS_NUMBER << " blocks." << std::endl;
    std::cout << "So, we need " << N_elements << " nodes." << std::endl;
    std::cout << "To be definite, there will be " << N_elements << " elements." << std::endl;
    std::cout << "And nearly " << 2 * N_elements << " nodes accordingly." << std::endl;
    std::cout << "(it is approximately " << 2 * N_elements * sizeof(Node) / 1024 / 1024 << " Mbytes in memory."
              << std::endl;

    ////////////////////////////////////////
    ///// Building
    ////////////////////////////////////////

    std::vector<int> underlied_array;
    std::vector<Task> tasks;

    int number_tasks = 100 * 1000;

//    read_input_from_file("../test.in", underlied_array, tasks);
    make_random_input(N_elements, number_tasks, underlied_array, tasks);

    if (N_elements != underlied_array.size())
    {
        N_elements = underlied_array.size();
    }
    if (number_tasks != tasks.size())
    {
        number_tasks = tasks.size();
    }
    std::vector<Node> nodes = makeDefaultTree(N_elements, underlied_array);
    update_full_tree(&nodes[0]);

#if WRITE_PNGS
    print_tree_to_png(&nodes[0], "tree_trivial", [](Node *v)
    { return v->trivial_i; });
#endif

    std::vector<Node> nodes_vEB = make_vEB_tree(N_elements, nodes);

    std::cout << "Finish building." << std::endl;

#if WRITE_PNGS
    print_tree_to_png(&nodes_vEB[0], "tree_vEB", [](Node *v)
    { return v->i_vEB; });
#endif

#if BENCHMARK_MODE
    ////////////////////////////////////////
    ///// Testing
    ////////////////////////////////////////

    int NUMBER_REPEATS = 50;
    double time_sum = 0;
    double time_sqr_sum = 0;

    std::cout << std::endl;

    for (int i = 0; i < NUMBER_REPEATS; i++)
    {
        clean_cache(i);
        auto tm_before = std::chrono::high_resolution_clock::now();
        benchmark_tree(&nodes[0], tasks);
        auto tm_after = std::chrono::high_resolution_clock::now();
        double cur = std::chrono::duration_cast<std::chrono::microseconds>(tm_after - tm_before).count();
        std::cout << cur << " ";
        time_sum += cur;
        time_sqr_sum += sqr(cur);
    }
    std::cout << std::endl;

    double classic_approx = time_sum * 1.0 / NUMBER_REPEATS;
    std::cout << "==========" << std::endl;
    std::cout << "Classic: "
              << classic_approx
              << " (sigma=" << sqrt(time_sqr_sum * 1.0 / NUMBER_REPEATS -
        sqr(classic_approx))
              << ")"
              << std::endl;
    std::cout << std::endl;

    //////////

    time_sum = 0;
    time_sqr_sum = 0;
    for (int i = 0; i < NUMBER_REPEATS; i++)
    {
        clean_cache(i);
        auto tm_before = std::chrono::high_resolution_clock::now();
        benchmark_tree(&nodes_vEB[0], tasks);
        auto tm_after = std::chrono::high_resolution_clock::now();
        double cur = std::chrono::duration_cast<std::chrono::microseconds>(tm_after - tm_before).count();
        std::cout << cur << " ";
        time_sum += cur;
        time_sqr_sum += cur * cur;
    }
    std::cout << std::endl;

    double vanEmdeBoas_approx = time_sum * 1.0 / NUMBER_REPEATS;
    std::cout << "van Emde Boas: "
              << vanEmdeBoas_approx
              << " (sigma = " << sqrt(time_sqr_sum * 1.0 / NUMBER_REPEATS -
        sqr(vanEmdeBoas_approx)) << ")"
              << std::endl;

    std::cout << "Performance difference in "
              << int(100 * (classic_approx * 1.0 / vanEmdeBoas_approx - 1)) << "%"
              << std::endl;
    std::cout << "==========" << std::endl;
    std::cout << "End." << std::endl;

#endif


    return 0;
}