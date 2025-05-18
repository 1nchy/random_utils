#include "random_caller.hpp"

#include "main.hpp"

#include <stdexcept>

constexpr unsigned _L = 20;
constexpr unsigned _N = 100;

struct kid {
public:
    static constexpr unsigned MAX_ENERGY = 10;
    kid() = default;
    auto add(int _x) -> int {
        _args.push_back(_x);
        _result = _x + bias(0);
        return _result;
    }
    auto add(int _x, int _y) -> int {
        _energy -= std::min(_energy, 1u);
        _args.push_back(_x);
        _args.push_back(_y);
        _result = _x + _y + bias(1);
        return _result;
    }
    auto add(int _x, int _y, int _z) -> int {
        _energy -= std::min(_energy, 2u);
        _args.push_back(_x);
        _args.push_back(_y);
        _args.push_back(_z);
        _result = _x + _y + _z + bias(2);
        return _result;
    }
    auto rest() -> void {
        _energy = std::min(_energy + _rou.rand(0, MAX_ENERGY / 2), MAX_ENERGY);
    }
    auto prepare() -> void {
        _args.clear();
        _result = 0;
    }
    auto check() const -> void {
        int _answer = 0;
        for (const auto _i : _args) {
            _answer += _i;
        }
        if (_answer != _result) {
            throw std::logic_error("error");
        }
    }
private:
    auto bias(unsigned _x) const -> int {
        if (_x == 0 || _energy >= MAX_ENERGY / 2) {
            return 0;
        }
        const int _factor = (_roi.rand(1, 0.5) == 0 ? -1 : 1);
        return _roi.rand(_x, 1.0 / std::max(_energy, 1u)) * _factor;
    }
private:
    std::vector<int> _args;
    int _result;
    icy::random_object<bool> _roi;
    icy::random_object<unsigned> _rou;
    unsigned _energy = MAX_ENERGY;
};

int main() {
    icy::random_caller<kid> _rc;
    _rc.enroll("add", static_cast<int(kid::*)(int)>(&kid::add));
    _rc.enroll("add2", static_cast<int(kid::*)(int,int)>(&kid::add));
    _rc.enroll("add3", static_cast<int(kid::*)(int,int,int)>(&kid::add));
    _rc.enroll("rest", &kid::rest);
    _rc.push_callback(&kid::prepare);
    _rc.push_callback(&kid::check);
    for (unsigned _i = 0; _i < _L; ++_i) {
        _rc.run(_N);
    }
    return 0;
}