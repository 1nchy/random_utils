#include "random_object.hpp"

#include <utility>

constexpr unsigned _N = 100;

int main() {
{
    icy::random_object<std::pair<unsigned, char>> _ro;
    icy::random_object<char>::static_bound("0-9");
    _ro.bound(
        std::forward_as_tuple(1, 4),
        std::forward_as_tuple("abc")
    );
    _ro.bound(
        std::forward_as_tuple(),
        std::forward_as_tuple("xyz")
    );
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert( 1<= _r.first && _r.first <= 3);
        assert(isdigit(_r.second));
    }
    icy::random_object<char>::static_unbound();
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert( 1<= _r.first && _r.first <= 3);
        assert('x' <= _r.second <= 'z');
    }
}
    return 0;
}