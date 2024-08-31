#ifndef _ICY_CONTAINER_WRAPPER_EXAMPLE_CAT_HPP_
#define _ICY_CONTAINER_WRAPPER_EXAMPLE_CAT_HPP_

#include <string>

struct cat {
    std::string cat3(const char& a, const char& b, const char& c);
    std::string cat2(const char a, const char b);
    std::string cat1(char& a);
    std::string cat0();
    char cat000(char& a) const;
    unsigned check() const;
private:
    int _total = 0;
};

#endif // _ICY_CONTAINER_WRAPPER_EXAMPLE_CAT_HPP_