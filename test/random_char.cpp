#include "random_object.hpp"

#include <cstdlib>
#include <unordered_set>

constexpr unsigned _N = 100;

int main() {
{
    icy::random_object<char> _ro;
    _ro.bound('a', 'f');
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        assert('a' <= _r && _r <= 'f');
    }
}
{
    icy::random_object<char> _ro;
    auto collection_check = [&_ro](const std::string& _s) -> bool {
        std::unordered_set<char> _a(_ro.collection().cbegin(), _ro.collection().cend());
        std::unordered_set<char> _b(_s.cbegin(), _s.cend());
        return _a == _b;
    };
    _ro.bound('2', '7');
    assert(collection_check("234567"));
    _ro.bound("2-7");
    assert(collection_check("234567"));
    _ro.bound("u-z");
    assert(collection_check("uvwxyz"));
    _ro.bound("c-a");
    assert(collection_check(""));
    _ro.bound("3-6a-c");
    assert(collection_check("3456abc"));
    _ro.bound("4-c");
    assert(collection_check("456789abc"));
    _ro.bound("5-B");
    assert(collection_check("56789AB"));
    _ro.bound("x-3");
    assert(collection_check("xyz0123"));
    _ro.bound("x-C");
    assert(collection_check("xyzABC"));
    _ro.bound("0-zA-Z");
    assert(_ro.collection().size() == 62);
    _ro.bound("0--f");
    assert(collection_check("f"));
    _ro.bound("3456789jqk");
    assert(collection_check("3456789jqk"));
    _ro.bound("3456789jqk-");
    assert(collection_check("3456789jqk-"));
    _ro.bound("--3456789jqk");
    assert(collection_check("456789jqk"));
    _ro.bound("--3456789-jqk");
    assert(collection_check("456789abcdefghijqk"));
}
    return 0;
}