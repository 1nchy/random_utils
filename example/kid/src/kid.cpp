#include "kid.hpp"

#include <iostream>
#include <numeric>

kid::kid(const kid& _rhs) : _args(_rhs._args), _result(_rhs._result), _energy(_rhs._energy) {
    increase_energy(1);
};
kid::kid(kid&& _rhs) : _args(std::move(_rhs._args)), _result(std::move(_rhs._result)), _energy(std::move(_rhs._energy)) {
    increase_energy(1);
};
auto kid::add(int _x) -> int {
    decrease_energy(0);
    _args.push_back(_x);
    _result = _x + bias(0);
    return _result;
}
auto kid::add(int _x, int _y) -> int {
    decrease_energy(1);
    _args.push_back(_x);
    _args.push_back(_y);
    _result = _x + _y + bias(1);
    return _result;
}
auto kid::add(int _x, int _y, int _z) -> int {
    decrease_energy(2);
    _args.push_back(_x);
    _args.push_back(_y);
    _args.push_back(_z);
    _result = _x + _y + _z + bias(2);
    return _result;
}
auto kid::increment(int& _x) -> void {
    decrease_energy(1);
    _args.push_back(_x);
    _args.push_back(1);
    _x += (1 + bias(1));
    _result = _x;
}
auto kid::rest() -> void {
    increase_energy(_rou.rand(0, MAX_ENERGY / 2));
}
auto kid::show() const -> void {
    if (!_args.empty()) {
        bool _first = true;
        for (const auto _i : _args) {
            if (!_first) std::cout << " + ";
            std::cout << _i; _first = false;
        }
        std::cout << " -> " << _result << ", ";
    }
    std::cout << "[" << _energy << "]" << std::endl;
}
auto kid::prepare() -> void {
    _args.clear();
    _result = 0;
}
auto kid::check() const -> void {
    const int _answer = std::accumulate(_args.cbegin(), _args.cend(), 0);
    if (_result < _answer) {
        throw std::logic_error("too small");
    }
    else if (_result > _answer) {
        throw std::logic_error("too big");
    }
}
auto kid::bias(unsigned _x) const -> int {
    if (_x == 0 || _energy >= MAX_ENERGY / 2) {
        return 0;
    }
    const int _factor = (_roi.rand(1, 0.5) == 0 ? -1 : 1);
    return _roi.rand(_x, 1.0 / std::max(_energy, 1u)) * _factor;
}
auto kid::increase_energy(unsigned _e) -> void {
    _energy = std::min(_energy + _e, MAX_ENERGY);
}
auto kid::decrease_energy(unsigned _e) -> void {
    _energy -= std::min(_energy, _e);
}