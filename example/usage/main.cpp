#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"
#include "cat.hpp"

int main() {
    icy::random_object<std::pair<const unsigned, std::string>> _ro1;
    for (size_t _i = 0; _i < 10; ++_i) {
        // const std::pair<unsigned, std::string>& _p = _ro1.rand(
        const auto& _p = _ro1.rand(
            // std::forward_as_tuple(0, 4),
            // std::forward_as_tuple(2, 5, 'A', 'F')
        );
        icy::container::operator<<(std::cout, _p);
        std::cout << std::endl;
    }
    icy::random_object<std::tuple<unsigned, char, std::string>> _ro2;
    for (size_t _i = 0; _i < 10; ++_i) {
        // const std::tuple<unsigned, char, std::string>& _p = _ro2.rand(
        const auto& _p = _ro2.rand(
            std::forward_as_tuple(0, 4),
            std::forward_as_tuple(),
            std::forward_as_tuple(2, 5, 'A', 'F')
        );
        icy::container::operator<<(std::cout, _p);
        std::cout << std::endl;
    }
    // icy::random_object<int>::lb = -1;
    // icy::random_integral_object<int>::ub = 3;
    // icy::container::wrapper<sum> _ws;
    // _ws.enroll("sum3", &sum::sum3);
    // _ws.enroll("sum2", &sum::sum2);
    // _ws.enroll("sum1", &sum::sum1);
    // _ws.enroll("sum_pair", &sum::sum_pair);
    // _ws.enroll("sum0", &sum::sum0, 3.0);
    // // _ws.call_with_random_args("sum3");
    // // _ws.call_with_random_args("sum1");
    // // _ws.call_with_random_args("sum0");
    // // auto _r2 = _ws.call<int>("sum2", 2, 4);
    // _ws.log_file("sum.txt");
    // _ws.enable_log();
    // _ws.run(20);

    // icy::container::wrapper<cat> _wc;
    // icy::random_object<char>::lb = 'a';
    // icy::random_object<char>::ub = 'f';
    // _wc.enroll("cat3", &cat::cat3);
    // _wc.enroll("cat2", &cat::cat2);
    // _wc.enroll("cat1", &cat::cat1);
    // _wc.enroll("cat0", &cat::cat0, 3.0);
    // _wc.enroll("cat000", &cat::cat000);
    // char _c;
    // // _wc.call<std::string>("cat1", _c);
    // // _wc.call<std::string>("cat0");
    // // _wc.call<char>("cat000", _c);
    // _wc.log_file("main.txt");
    // _wc.enable_log();
    // _wc.run(30);
    return 0;


}