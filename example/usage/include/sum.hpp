#ifndef _ICY_CONTAINER_TEST_EXAMPLE_SUM_HPP_
#define _ICY_CONTAINER_TEST_EXAMPLE_SUM_HPP_

struct sum {
    int sum3(const int& a, const int& b, const int& c);
    int sum2(const int a, const int b);
    int sum1(int& a);
    int sum0();
    unsigned check() const;
private:
    int _total = 0;
};

#endif // _ICY_CONTAINER_TEST_EXAMPLE_SUM_HPP_