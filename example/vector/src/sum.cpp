#include "sum.hpp"

#include <iostream>

int sum::sum3(int a, int b, int c) {
    std::cout << "sum3(" << a << "+" << b << "+" << c << ")" << std::endl;
    return a + c;
}
int sum::sum2(int a, int b) {
    std::cout << "sum2(" << a << "+" << b << ")" << std::endl;
    return a + b;
}
int sum::sum1(int a) {
    std::cout << "sum1(" << a << ")" << std::endl;
    return a;
}
int sum::sum0() {
    std::cout << "sum()" << std::endl;
    return 0;
}