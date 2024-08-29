#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"

int main() {
    icy::container::wrapper<sum> _w;
    _w.enroll("sum3", &sum::sum3);
    _w.enroll("sum2", &sum::sum2);
    _w.enroll("sum1", &sum::sum1);
    _w.enroll("sum0", &sum::sum0, 3.0);
    // _w.random_call("sum3");
    // _w.random_call("sum1");
    // _w.random_call("sum0");
    // auto _r2 = _w.call<int>("sum2", 2, 4);
    _w.run(30);
    return 0;
}