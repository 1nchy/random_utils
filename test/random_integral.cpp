#include "random_object.hpp"

#include "main.hpp"

constexpr unsigned _N = 100;
icy::random_object<unsigned> _rou;

int main() {
    _rou.bound(2, 5);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rou.rand();
        EXPECT_TRUE(2 <= _r && _r < 5);
    }
    EXPECT_TRUE(_rou.lower_bound() == 2);
    _rou.static_bound(1, 3);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rou.rand();
        EXPECT_TRUE(1 <= _r && _r < 3);
    }
    _rou.bound(4, 7);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rou.rand();
        EXPECT_TRUE(1 <= _r && _r < 3);
    }
    EXPECT_TRUE(_rou.lower_bound() == 1);
    EXPECT_TRUE(_rou.upper_bound() == 3);
    _rou.static_unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rou.rand();
        EXPECT_TRUE(4 <= _r && _r < 7);
    }
    return 0;
}