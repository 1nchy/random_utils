#include "random_object.hpp"

#include "main.hpp"

constexpr unsigned _N = 100;

int main() {
{
    icy::random_object<float> _ro;
    _ro.bound(2, 5);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(2 <= _r && _r < 5);
    }
}
{
    icy::random_object<double> _ro;
    _ro.static_bound(-1, 3);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(-1 <= _r && _r < 3);
    }
    _ro.bound(4, 7);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(-1 <= _r && _r < 3);
    }
}
{
    icy::random_object<long double> _ro;
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(-1 <= _r && _r < 3);
    }
    _ro.static_unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(0 <= _r && _r < 1);
    }
}
    return 0;
}