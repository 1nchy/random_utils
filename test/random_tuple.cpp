#include "random_object.hpp"
#include "container_wrapper.hpp"

#include <tuple>
#include <string>

constexpr unsigned _N = 100;

int main() {
{
    icy::random_object<std::tuple<unsigned, float, std::string>> _ro;
    _ro.bound(
        std::forward_as_tuple(2, 4),
        std::forward_as_tuple(),
        std::forward_as_tuple(3, 7, "a-f")
    );
    _ro.bound<1>(-1.0, 0.0);
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _p = _ro.rand();
        const auto _u = std::get<0>(_p);
        assert(2 <= _u && _u < 4);
        const auto _f = std::get<1>(_p);
        assert(-1.0 <= _f && _f <= 0.0);
        const auto _s = std::get<2>(_p);
        assert(3 <= _s.size() && _s.size() < 7);
        assert(std::all_of(_s.cbegin(), _s.cend(), [](char _c) {
            return 'a' <= _c && _c <= 'f';
        }));
    }
}
{
    icy::random_object<std::tuple<char>> _ro;
    _ro.bound(
        std::forward_as_tuple("aeiou")
    );
    _ro.bound<0>("abcde");
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _p = _ro.rand();
        const auto _c = std::get<0>(_p);
        assert('a' <= _c && _c <= 'e');
    }
}
    return 0;
}