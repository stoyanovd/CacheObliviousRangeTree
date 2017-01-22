
#ifndef CACHEOBLIVIOUSRANGETREE_TASK_H
#define CACHEOBLIVIOUSRANGETREE_TASK_H

struct Task
{
    bool is_update;
    // union will be suitable...
    int l, r, ans;
    bool ans_is_filled;
    int pos, new_val;
};


#endif //CACHEOBLIVIOUSRANGETREE_TASK_H
