#include "random_object.hpp"
#include "container_wrapper.hpp"

#include "sum.hpp"
#include "cat.hpp"

int main() {
    icy::random_object<const unsigned>::ub = 4;
    icy::random_object<char>::lb = 'a';
    icy::random_object<char>::ub = 'f';
    icy::random_object<std::string>::lb = 4;
    icy::random_object<std::string>::ub = 7;
    icy::random_object<bool>::lb = 4.0;
    icy::random_object<bool>::ub = 2.5;
    size_t _false_count = 0ul;
    size_t _true_count = 0ul;
    icy::random_object<bool> _ro0;
    for (size_t _i = 0; _i < 100000; ++_i) {
        if (_ro0.rand()) ++_true_count;
        else ++_false_count;
    }
    std::cout << "true: " << _true_count << ", false: " << _false_count << std::endl;
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
            std::forward_as_tuple(),
            std::forward_as_tuple(),
            std::forward_as_tuple(2, 5, 'A', 'F')
        );
        icy::container::operator<<(std::cout, _p);
        std::cout << std::endl;
    }
    icy::random_object<int>::lb = -1;
    icy::random_object<int>::ub = 3;
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
    icy::random_object<char>::lb = 'a';
    icy::random_object<char>::ub = 'f';
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