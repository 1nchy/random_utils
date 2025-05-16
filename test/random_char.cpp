#include "random_object.hpp"

#include "main.hpp"

#include <cstdlib>
#include <unordered_set>

constexpr unsigned _N = 100;
icy::random_object<char> _roc;

auto collection_equal(const std::string& _s) -> bool {
    return (
        std::unordered_set<char>(_roc.collection().cbegin(), _roc.collection().cend()) == 
        std::unordered_set<char>(_s.cbegin(), _s.cend())
    );
}

int main() {
    _roc.bound('a', 'f');
    for (size_t _i = 0; _i != _N; ++_i) {
        const auto _r = _roc.rand();
        EXPECT_TRUE('a' <= _r && _r <= 'f');
    }
    _roc.bound('2', '7');
    EXPECT_TRUE(collection_equal("234567"));
    _roc.bound("2-7");
    EXPECT_TRUE(collection_equal("234567"));
    _roc.bound("u-z");
    EXPECT_TRUE(collection_equal("uvwxyz"));
    _roc.bound("c-a");
    EXPECT_TRUE(collection_equal(""));
    _roc.bound("3-6a-c");
    EXPECT_TRUE(collection_equal("3456abc"));
    _roc.bound("4-c");
    EXPECT_TRUE(collection_equal("456789abc"));
    _roc.bound("5-B");
    EXPECT_TRUE(collection_equal("56789AB"));
    _roc.bound("x-3");
    EXPECT_TRUE(collection_equal("xyz0123"));
    _roc.bound("x-C");
    EXPECT_TRUE(collection_equal("xyzABC"));
    _roc.bound("0-zA-Z");
    EXPECT_TRUE(_roc.collection().size() == 62);
    _roc.bound("0--f");
    EXPECT_TRUE(collection_equal("f"));
    _roc.bound("3456789jqk");
    EXPECT_TRUE(collection_equal("3456789jqk"));
    _roc.bound("3456789jqk-");
    EXPECT_TRUE(collection_equal("3456789jqk-"));
    _roc.bound("--3456789jqk");
    EXPECT_TRUE(collection_equal("456789jqk"));
    _roc.bound("--3456789-jqk");
    EXPECT_TRUE(collection_equal("456789abcdefghijqk"));
    return 0;
}