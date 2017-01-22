#include <iostream>
#include <gtest/gtest.h>

#include "RangeTree.h"
#include "Task.h"
#include "InputCreator.h"


void test_tree(Node *root, std::vector<Task> tasks)
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
            ASSERT_EQ(task.ans_is_filled, true) << "Ans must be filled in test file.";
            ASSERT_EQ(ans, task.ans) << "RangeTree returned not correct answer "
                                     << "(l = " << task.l
                                     << ", r = " << task.r << ")";
        }
    }
}

TEST(CheckTreeCorrectness, ClassicTree)
{
    std::vector<int> underlied_array;
    std::vector<Task> tasks;
    read_input_from_file("../tests/test.in", underlied_array, tasks);

    std::vector<Node> nodes = RangeTree::makeDefaultTree(underlied_array.size(),
                                                         underlied_array);
    test_tree(&nodes[0], tasks);
}

TEST(CheckTreeCorrectness, vanEmdeBoasTree)
{
    std::vector<int> underlied_array;
    std::vector<Task> tasks;
    read_input_from_file("../tests/test.in", underlied_array, tasks);

    std::vector<Node> nodes = RangeTree::makeDefaultTree(underlied_array.size(),
                                                         underlied_array);

    std::vector<Node> nodes_vEB = RangeTree::make_vEB_tree(underlied_array.size(),
                                                           nodes);
    test_tree(&nodes_vEB[0], tasks);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}