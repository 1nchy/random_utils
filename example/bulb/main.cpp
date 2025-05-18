#include "bulb.hpp"
#include "random_caller.hpp"

#include <iostream>

constexpr size_t _N = 20;

int main() {
    icy::random_caller<bulb> _rcb;
    icy::random_object<size_t>::static_bound(1, 3);
    _rcb.enroll("light_on", &bulb::light_on, 3.0);
    _rcb.enroll("light_out", &bulb::light_out, 3.0);
    _rcb.enroll("light", &bulb::light, 1.0);
    _rcb.enroll("fix", static_cast<void(bulb::*)()>(&bulb::fix), 0.5);
    _rcb.enroll("fix2", static_cast<void(bulb::*)(size_t)>(&bulb::fix), 0.5);
    _rcb.enroll("remains", &bulb::remains, 1.0);
    _rcb.push_callback(&bulb::sweep);
    _rcb.push_callback(&bulb::check);
    _rcb.run(_N);
    const size_t _L = _rcb.get_commands().size();
    for (size_t _i = 0; _i != _L; ++_i) {
        std::cout << _rcb.get_commands()[_i] << _rcb.get_arguments()[_i] << std::endl;
    }
    std::cout << _rcb.get_exception() << std::endl;
    return 0;
}