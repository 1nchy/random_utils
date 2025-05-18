#include "random_caller.hpp"

#include "main.hpp"

size_t _callback_count = 0;

auto add(int&& _x, int&& _y) -> int { return _x + _y; }
auto add(const int& _x, const int& _y) -> int { return _x + _y; }

auto increment(int& _x) -> void { _x += 1; }
auto decrement(int& _x) -> void { _x -= 1; }

auto poll1() -> void {
    ++_callback_count;
}
auto poll2(unsigned) -> void {
    ++_callback_count;
}

int main() {
    icy::random_caller _rc;
    EXPECT_TRUE(_rc.enroll("add", static_cast<int(*)(int&&,int&&)>(add)));
    EXPECT_TRUE(_rc.enroll("add2", static_cast<int(*)(const int&,const int&)>(add)));
    EXPECT_TRUE(_rc.enroll("increment", increment));
    EXPECT_TRUE(_rc.enroll("decrement", decrement));
    _rc.push_callback(poll1);
    _rc.push_callback(poll2);
    EXPECT_TRUE(_rc.run(10));
    EXPECT_EQ(_callback_count, 20);
    return 0;
}