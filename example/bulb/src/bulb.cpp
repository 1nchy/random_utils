#include "bulb.hpp"

#include <stdexcept>

bulb::bulb() { _rob.bound(1, 0.8); }
auto bulb::light_on() -> void {
    if (!_light) {
        if (_count < _life) {
            _count += _rob.rand();
            _light = true;
        }
    }
}
auto bulb::light_out() -> void {
    _light = false;
}
auto bulb::light() const -> bool {
    return _light;
}
auto bulb::fix(size_t _l) -> void {
    _count = std::min(_count, _l);
}
auto bulb::remains() const -> size_t {
    return _life - _count;
}
auto bulb::sweep() -> void {
    if (_rob.rand(1, 0.99) == 0) {
        _count = _life + 1;
    }
}
auto bulb::check() const -> void {
    if (_count == _life) {
        throw std::logic_error("use out");
    }
    else if (_count > _life) {
        throw std::logic_error("break while sweeping");
    }
}