#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"

int main() {
    icy::container::wrapper<sum> _w;
    _w.enroll("sum3", &sum::sum3);
    _w.enroll("sum1", &sum::sum1);
    _w.enroll("sum0", &sum::sum0);
    _w.random_call("sum3");
    _w.random_call("sum1");
    _w.random_call("sum0");
    return 0;
}