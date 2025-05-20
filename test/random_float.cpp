#include "random_object.hpp"

#include "main.hpp"

constexpr unsigned _N = 100;
icy::random_object<float> _rof;

int main() {
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rof.rand();
        EXPECT_TRUE(0 <= _r && _r < 1);
    }
    _rof.bound(2, 5);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rof.rand();
        EXPECT_TRUE(2 <= _r && _r < 5);
    }
    _rof.static_bound(-1, 3);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rof.rand();
        EXPECT_TRUE(2 <= _r && _r < 5);
    }
    _rof.bound(4, 7);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rof.rand();
        EXPECT_TRUE(4 <= _r && _r < 7);
    }
    _rof.unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _rof.rand();
        EXPECT_TRUE(-1 <= _r && _r < 3);
    }
    return 0;
}