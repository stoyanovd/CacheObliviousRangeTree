#ifndef CACHEOBLIVIOUSRANGETREE_TREECREATOR_H
#define CACHEOBLIVIOUSRANGETREE_TREECREATOR_H

#include <vector>
#include "Task.h"

void read_input_from_file(std::string const &file,
                          std::vector<int> &array_from_file,
                          std::vector<Task> &tasks_from_file);

void make_random_input(int N_elements, int number_tests,
                       std::vector<int> &array,
                       std::vector<Task> &tasks);

#endif //CACHEOBLIVIOUSRANGETREE_TREECREATOR_H
