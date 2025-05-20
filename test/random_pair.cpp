#include "random_object.hpp"

#include "main.hpp"

#include <utility>

constexpr unsigned _N = 100;
icy::random_object<std::pair<unsigned, char>> _ro;

int main() {
    icy::random_object<char>::static_bound("0-9");
    _ro.bound(
        std::forward_as_tuple(1, 4),
        std::forward_as_tuple("abc")
    );
    _ro.bound(
        std::forward_as_tuple(),
        std::forward_as_tuple("xyz")
    );
    // ((1,4), "xyz")
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        EXPECT_TRUE( 1<= _r.first && _r.first <= 3);
        EXPECT_TRUE('x' <= _r.second && _r.second <= 'z');
    }
    _ro.unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        EXPECT_TRUE( 0 <= _r.first && _r.first < 2);
        EXPECT_TRUE('0' <= _r.second && _r.second <= '9');
    }
    return 0;
}