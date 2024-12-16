#include "random_object.hpp"

constexpr unsigned _N = 100;

int main() {
{
    icy::random_object<unsigned> _ro;
    _ro.bound(2, 5);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert(2 <= _r && _r < 5);
    }
    assert(_ro.lower_bound() == 2);
}
{
    icy::random_object<int> _ro;
    _ro.static_bound(-1, 3);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert(-1 <= _r && _r < 3);
    }
    _ro.bound(4, 7);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert(-1 <= _r && _r < 3);
    }
    assert(_ro.lower_bound() == -1);
}
{
    icy::random_object<int> _ro;
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert(-1 <= _r && _r < 3);
    }
    assert(_ro.upper_bound() == 3);
    _ro.static_unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert(0 <= _r && _r < 2);
    }
}
    return 0;
}