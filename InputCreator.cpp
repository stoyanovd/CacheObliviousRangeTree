#include <iostream>
#include <fstream>

#include "InputCreator.h"

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

const int MAX_ELEMENT_VALUE = 17239;

void make_random_input(int N_elements, int number_tests,
                       std::vector<int> &array,
                       std::vector<Task> &tasks)
{
    for (int i = 0; i < N_elements; i++)
    {
        array.push_back(rand() % MAX_ELEMENT_VALUE);
    }
    for (int i = 0; i < number_tests; i++)
    {
        Task task = Task();
        task.is_update = (bool) (rand() % 2);
        if (task.is_update)
        {
            task.pos = rand() % N_elements;
            task.new_val = rand() % MAX_ELEMENT_VALUE;
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
