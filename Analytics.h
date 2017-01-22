#ifndef CACHEOBLIVIOUSRANGETREE_ANALYTICS_H
#define CACHEOBLIVIOUSRANGETREE_ANALYTICS_H

#include <chrono>
#include <vector>
#include <cmath>

template<class T>
inline T sqr(T x)
{
    return x * x;
}

struct Analytics
{
    static const int NUMBER_REPEATS = 10;
    typedef std::chrono::milliseconds time_spec;

    double time_sum = 0;

    double time_sqr_sum = 0;
    std::vector<int> times;

    void add_time(double time)
    {
        times.push_back(time);
        time_sum += time;
        time_sqr_sum += time * time;
    }

    double get_average()
    {
        return time_sum * 1.0 / NUMBER_REPEATS;
    }

    double get_sigma()
    {
        return sqrt(time_sqr_sum * 1.0 / NUMBER_REPEATS - sqr(get_average()));
    }

    double get_sigma_to_average_percents()
    {
        return int(100.0 * get_sigma() / get_average());
    }

};

#endif //CACHEOBLIVIOUSRANGETREE_ANALYTICS_H
