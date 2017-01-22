#include <iostream>
#include <fstream>

#include <glog/logging.h>

#include "Task.h"
#include "RangeTree.h"
#include "TreePrinter.h"
#include "Analytics.h"
#include "InputCreator.h"

void benchmark_tree(Node *root, std::vector<Task> tasks)
{
    for (auto task : tasks)
    {
        if (task.is_update)
        {
            RangeTree::update(root, task.pos, task.new_val);
        }
        else
        {
            int ans = RangeTree::query(root, task.l, task.r);
            DCHECK(!task.ans_is_filled || (ans == task.ans)) << " Wrong answer.";
        }
    }
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

void benchmark_it(Node *root, Analytics &analytics, const std::string &tree_name,
                  const std::vector<Task> &tasks)
{
    for (int i = 0; i < analytics.NUMBER_REPEATS; i++)
    {
        clean_cache(i);
        auto tm_before = std::chrono::high_resolution_clock::now();
        benchmark_tree(root, tasks);
        auto tm_after = std::chrono::high_resolution_clock::now();

        double cur = std::chrono::duration_cast<Analytics::time_spec>(tm_after - tm_before).count();
        analytics.add_time(cur);
    }

    std::cout << tree_name << ": " << analytics.get_average()
              << " (sigma=" << analytics.get_sigma()
              << ", it is " << analytics.get_sigma_to_average_percents() << "%)"
              << std::endl << std::endl;
}

// should not be run with large trees
const bool WRITE_PNGS = 0;

const bool GET_INPUT_FROM_FILE = 0;

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);

    // L1 size
//    const int N_in_block = 32 * 1024 / sizeof(Node);
    // L2 size
//    const int N_in_block = 256 * 1024 / sizeof(Node);
    // L3 size
    const int N_in_block = 6144 * 1024 / sizeof(Node);

    const int BLOCKS_NUMBER = 90;
    int N_elements = N_in_block * BLOCKS_NUMBER;
//    N_elements = 11;

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

    int number_tasks = 30 * 100 * 1000;

    if (GET_INPUT_FROM_FILE)
    {
        read_input_from_file("../tests/test.in", underlied_array, tasks);
        N_elements = underlied_array.size();
        number_tasks = tasks.size();
    }
    else
    {
        make_random_input(N_elements, number_tasks, underlied_array, tasks);
    }
    std::vector<Node> nodes = RangeTree::makeDefaultTree(N_elements, underlied_array);
    std::vector<Node> nodes_vEB = RangeTree::make_vEB_tree(N_elements, nodes);

    std::cout << "Finish building." << std::endl;
    std::cout << "==========" << std::endl << std::endl;

    ////////////////////////////////////////
    ///// Write images
    ////////////////////////////////////////

    if (WRITE_PNGS)
    {
        print_tree_to_png(&nodes[0], "tree_trivial", [](Node *v)
        { return v->trivial_i; });
        print_tree_to_png(&nodes_vEB[0], "tree_vEB", [](Node *v)
        { return v->i_vEB; });
    }

    ////////////////////////////////////////
    ///// Testing
    ////////////////////////////////////////

    std::cout << "Run " << number_tasks << " tasks on each tree." << std::endl;
    std::cout << "Result is time to complete this pack (in millis)." << std::endl;
    std::cout << "Average from " << Analytics::NUMBER_REPEATS << " repeats." << std::endl;
    std::cout << std::endl;

    Analytics classic = Analytics();
    Analytics vanEmdeBoas = Analytics();

    benchmark_it(&nodes[0], classic, "Classic", tasks);
    benchmark_it(&nodes_vEB[0], vanEmdeBoas, "van Emde Boas", tasks);


    int improve = int(100.0 * (classic.get_average() * 1.0 / vanEmdeBoas.get_average() - 1));
    std::cout << "Performance difference in " << improve << "%" << std::endl;
    std::cout << std::endl << "==========" << std::endl;
    std::cout << "End." << std::endl;

    return 0;
}