#include "cat.hpp"

#include <iostream>

std::string cat::cat3(const char& a, const char& b, const char& c) {
    std::cout << "cat3(" << a << "+" << b << "+" << c << ")" << std::endl;
    _total += 3;
    return std::string(1, a) + std::string(1, b) + std::string(1, c);
}
std::string cat::cat2(const char a, const char b) {
    std::cout << "cat2(" << a << "+" << b << ")" << std::endl;
    _total += 2;
    return std::string(1, a) + std::string(1, b);
}
std::string cat::cat1(char& a) {
    std::cout << "cat1(" << a << ")" << std::endl;
    _total += 1;
    return std::string(1, a);
}
std::string cat::cat0() {
    std::cout << "cat()" << std::endl;
    return "";
}
char cat::cat000(char& a) const {
    return 0;
}
unsigned cat::check() const {
    if (_total > 15) {
        return 1l;
    }
    return 0l;
}