#include "sum.hpp"

#include <iostream>

int sum::sum3(const int& a, const int& b, const int& c) {
    std::cout << "sum3(" << a << "+" << b << "+" << c << ")" << std::endl;
    _total += (a + b + c);
    return a + b + c;
}
int sum::sum2(const int a, const int b) {
    std::cout << "sum2(" << a << "+" << b << ")" << std::endl;
    _total += (a + b);
    return a + b;
}
int sum::sum1(int& a) {
    std::cout << "sum1(" << a << ")" << std::endl;
    _total += (a);
    return a;
}
int sum::sum0() {
    std::cout << "sum()" << std::endl;
    return 0;
}
unsigned sum::check() const {
    if (_total > 15) {
        return 1l;
    }
    return 0l;
}