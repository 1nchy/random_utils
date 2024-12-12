#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"
#include "cat.hpp"

int main() {
    size_t _false_count = 0ul;
    size_t _true_count = 0ul;
    icy::random_object<bool> _ro0;
    _ro0.bound(1, 0.3);
    for (size_t _i = 0; _i < 100000; ++_i) {
        if (_ro0.rand()) ++_true_count;
        else ++_false_count;
    }
    std::cout << "true: " << _true_count << ", false: " << _false_count << std::endl;
    icy::random_object<std::string> _ro1;
    _ro1.bound(4, 7);
    _ro1.bound("0-f");
    for (size_t _i = 0; _i < 10; ++_i) {
        const auto& _p = _ro1.rand();
        std::operator<<(std::cout, _p);
        std::cout << std::endl;
    }
    icy::random_object<std::tuple<unsigned, char, std::string>> _ro2;
    for (size_t _i = 0; _i < 10; ++_i) {
        _ro2.bound(
            std::forward_as_tuple(2, 4),
            std::forward_as_tuple('a', 'f'),
            std::forward_as_tuple(2, 5, "0-f")
        );
        const auto& _p = _ro2.rand();
        // const auto& _p = _ro2.rand(
        //     std::forward_as_tuple(2, 4),
        //     std::forward_as_tuple('a', 'f'),
        //     std::forward_as_tuple(2, 5, 'A', 'F')
        // );
        icy::container::operator<<(std::cout, _p);
        std::cout << std::endl;
    }
    icy::random_object<int>::static_bound(-1, 3);
    icy::container::wrapper<sum> _ws;
    _ws.enroll("sum3", &sum::sum3);
    _ws.enroll("sum2", &sum::sum2);
    _ws.enroll("sum1", &sum::sum1);
    _ws.enroll("sum_pair", &sum::sum_pair);
    _ws.enroll("sum0", &sum::sum0, 3.0);
    _ws.enroll_copy_construtor();
    _ws.log_file("sum.txt");
    _ws.enable_log();
    _ws.run(40);

    icy::container::wrapper<cat> _wc;
    icy::random_object<char>::static_bound("a-f");
    _wc.enroll("cat3", &cat::cat3);
    _wc.enroll("cat2", &cat::cat2);
    _wc.enroll("cat1", &cat::cat1);
    _wc.enroll("cat0", &cat::cat0, 3.0);
    _wc.enroll("cat000", &cat::cat000);
    _wc.log_file("cat.txt");
    _wc.enable_log();
    _wc.run(30);
    return 0;


}