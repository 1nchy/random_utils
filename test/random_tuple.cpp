#include "random_object.hpp"

#include "main.hpp"

#include <tuple>
#include <string>

constexpr unsigned _N = 100;

int main() {
    icy::random_object<std::tuple<unsigned, float, std::string>> _ro;
    icy::random_object<float>::static_bound(2.0, 3.0);
    icy::random_object<char>::static_bound('A', 'F');
    _ro.bound(
        std::forward_as_tuple(2, 4),
        std::forward_as_tuple(),
        std::forward_as_tuple(3, 7, "a-f")
    );
    _ro.bound<1>(-1.0, 0.0);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _p = _ro.rand();
        const auto _u = std::get<0>(_p);
        EXPECT_TRUE(2 <= _u && _u < 4);
        const auto _f = std::get<1>(_p);
        EXPECT_TRUE(-1.0 <= _f && _f <= 0.0);
        const auto _s = std::get<2>(_p);
        EXPECT_TRUE(3 <= _s.size() && _s.size() < 7);
        EXPECT_TRUE(std::all_of(_s.cbegin(), _s.cend(), [](char _c) {
            return 'a' <= _c && _c <= 'f';
        }));
    }
    _ro.unbound<2>();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _p = _ro.rand();
        const auto _s = std::get<2>(_p);
        EXPECT_TRUE(0 <= _s.size() && _s.size() < 2);
        EXPECT_TRUE(std::all_of(_s.cbegin(), _s.cend(), [](char _c) {
            return 'A' <= _c && _c <= 'F';
        }));
    }
    _ro.unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _p = _ro.rand();
        const auto _u = std::get<0>(_p);
        EXPECT_TRUE(0 <= _u && _u < 2);
        const auto _f = std::get<1>(_p);
        EXPECT_TRUE(2.0 <= _f && _f <= 3.0);
        const auto _s = std::get<2>(_p);
        EXPECT_TRUE(0 <= _s.size() && _s.size() < 2);
        EXPECT_TRUE(std::all_of(_s.cbegin(), _s.cend(), [](char _c) {
            return 'A' <= _c && _c <= 'F';
        }));
    }
    return 0;
}