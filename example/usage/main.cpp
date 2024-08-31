#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"
#include "cat.hpp"

int main() {
    // icy::container::wrapper<sum> _w;
    // _w.enroll("sum3", &sum::sum3);
    // _w.enroll("sum2", &sum::sum2);
    // _w.enroll("sum1", &sum::sum1);
    // _w.enroll("sum0", &sum::sum0, 3.0);
    // // _w.call_with_random_args("sum3");
    // // _w.call_with_random_args("sum1");
    // // _w.call_with_random_args("sum0");
    // // auto _r2 = _w.call<int>("sum2", 2, 4);
    // _w.run(30);

    icy::container::wrapper<cat> _w;
    _w.enroll("cat3", &cat::cat3);
    _w.enroll("cat2", &cat::cat2);
    _w.enroll("cat1", &cat::cat1);
    _w.enroll("cat0", &cat::cat0, 3.0);
    _w.enroll("cat000", &cat::cat000);
    char _c;
    _w.call<std::string>("cat1", _c);
    _w.call<std::string>("cat0");
    _w.call<char>("cat000", _c);
    _w.run(30);
    return 0;
}