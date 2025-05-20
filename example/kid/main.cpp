#include "kid.hpp"
#include "random_caller.hpp"

#include <iostream>

constexpr size_t _N = 25;

int main() {
    icy::random_caller<kid> _rc;
    icy::random_object<int>::static_bound(0, 10);
    _rc.enroll("add", static_cast<int(kid::*)(int)>(&kid::add));
    _rc.enroll("add2", static_cast<int(kid::*)(int,int)>(&kid::add), 1.0,
        std::forward_as_tuple(10, 15),
        std::forward_as_tuple(20, 25)
    );
    _rc.enroll("add3", static_cast<int(kid::*)(int,int,int)>(&kid::add));
    _rc.enroll("rest", &kid::rest);
    _rc.enroll_copy_construtor();
    _rc.enroll_move_construtor();
    _rc.push_callback(&kid::show);
    _rc.push_callback(&kid::check);
    _rc.push_callback(&kid::prepare);
    _rc.run(_N);
    const size_t _L = _rc.get_commands().size();
    for (size_t _i = 0; _i != _L; ++_i) {
        std::cout << _rc.get_commands()[_i] << _rc.get_arguments()[_i] << std::endl;
    }
    std::cout << _rc.get_exception() << std::endl;
    return 0;
}