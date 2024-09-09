#ifndef _ICY_CONTAINER_WRAPPER_EXAMPLE_SUM_HPP_
#define _ICY_CONTAINER_WRAPPER_EXAMPLE_SUM_HPP_

#include <utility>

struct sum {
    int sum3(const int& a, const int& b, const int& c);
    int sum2(const int a, const int b);
    int sum1(int& a);
    int sum_pair(const std::pair<unsigned, float>& _p);
    int sum0() const;
    unsigned check() const;
private:
    int _total = 0;
};

#endif // _ICY_CONTAINER_WRAPPER_EXAMPLE_SUM_HPP_