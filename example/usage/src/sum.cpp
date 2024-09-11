#include "sum.hpp"

#include <iostream>

sum::sum() {
    printf("constructor\n");
    _total = 0;
}
sum::sum(const sum& _rhs) {
    _total = _rhs._total - 1;
    printf("copy constructor (%d)\n", _total);
}
sum::~sum() {
    printf("destructor\n");
}
int sum::sum3(const int& a, const int& b, const int& c) {
    _total += (a + b + c);
    return a + b + c;
}
int sum::sum2(const int a, const int b) {
    _total += (a + b);
    return a + b;
}
int sum::sum1(int& a) {
    _total += (a);
    return a;
}
int sum::sum_pair(const std::pair<unsigned, float>& _p) {
    _total += (_p.first + _p.second);
    return _p.first + _p.second;
}
int sum::sum0() const {
    return 0;
}
unsigned sum::check() const {
    if (_total > 15) {
        return 1l;
    }
    return 0l;
}