#include "random_object.hpp"

#include "main.hpp"

#include <numeric>

constexpr unsigned _N = 100000;

bool deviation_in(double _x, double _std, double _p) {
    return std::abs(_x / _std - 1) < _p;
}

void verify_probability_density(const std::vector<double>& _d) {
    icy::random_object<void> _ro;
    _ro.density(_d.cbegin(), _d.cend());
    std::vector<unsigned> _result(_d.size(), 0);
    for (unsigned _i = 0; _i != _N; ++_i) {
        const auto _r = _ro.rand();
        icy_assert(0 <= _r && _r <= _d.size());
        ++_result[_r];
    }
    const auto _sum = std::accumulate(_d.cbegin(), _d.cend(), 0.0);
    for (size_t _i = 0; _i != _d.size(); ++_i) {
        icy_assert(deviation_in(_result[_i], _d[_i] * _N / _sum, 0.1));
    }
}

int main() {
    verify_probability_density({1,2,3,4});
    verify_probability_density({3,2,1,3,4});
    verify_probability_density({3.5,2.5,1.15,0.1,3.25,4.5});
    return 0;
}