#include "random_object.hpp"

#include <ctime>
#include <cstdlib>
#include <cstdarg>
#include <unordered_set>

namespace icy {

random_object_impl<bool>::random_object_impl() : _rd(), _gen(_rd()) {}
random_object_impl<bool>::random_object_impl(uint_fast32_t _seed) : _gen(_seed) {}
auto random_object_impl<bool>::rand(obj_type _n, bound_type _p) const -> obj_type {
    assert(0.0 <= _p && _p <= 1.0);
    std::binomial_distribution<obj_type> _dist(_n, _p);
    return _dist(_gen);
}
auto random_object_impl<bool>::rand() const -> obj_type {
    return rand(count(), probability());
}
auto random_object_impl<bool>::count() const -> obj_type {
    return (_s_global_bound ? _s_n : _n);
}
auto random_object_impl<bool>::probability() const -> bound_type {
    return (_s_global_bound ? _s_p : _p);
}
void random_object_impl<bool>::bound(obj_type _n, bound_type _p) {
    this->_n = _n; this->_p = _p;
}
void random_object_impl<bool>::static_bound(obj_type _n, bound_type _p) {
    _s_n = _n; _s_p = _p; _s_global_bound = true;
}
void random_object_impl<bool>::static_unbound() {
    _s_global_bound = false;
}



random_object_impl<char>::random_object_impl() : _rd(), _gen(_rd()) {}
random_object_impl<char>::random_object_impl(uint_fast32_t _seed) : _gen(_seed) {}
void random_object_impl<char>::bound(const std::string& _s) {
    _collection = _M_range(_s);
}
void random_object_impl<char>::bound(bound_type _l, bound_type _u) {
    _collection = _M_range(_l, _u);
}
void random_object_impl<char>::static_bound(const std::string& _s) {
    _s_collection = _M_range(_s);
    _s_global_bound = true;
}
void random_object_impl<char>::static_bound(bound_type _l, bound_type _u) {
    _s_collection = _M_range(_l, _u);
    _s_global_bound = true;
}
void random_object_impl<char>::static_unbound() {
    _s_global_bound = false;
}
auto random_object_impl<char>::collection() const -> const std::string& {
    return (_s_global_bound ? _s_collection : _collection);
}
auto random_object_impl<char>::rand(const std::string& _s) const -> obj_type {
    const auto& _r = _M_range(_s);
    return _M_rand_from_string(_r);
}
auto random_object_impl<char>::rand(bound_type _l, bound_type _u) const -> obj_type {
    const auto& _r = _M_range(_l, _u);
    return _M_rand_from_string(_r);
}
auto random_object_impl<char>::rand() const -> obj_type {
    return _M_rand_from_string(collection());
}
auto random_object_impl<char>::_M_rand_from_string(const std::string& _s) const -> obj_type {
    if (_s.empty()) return obj_type(0);
    std::uniform_int_distribution<size_t> _dist(0, _s.size() - 1);
    return _s.at(_dist(_gen));
}
auto random_object_impl<char>::_M_range(const std::string& _s) -> const std::string {
    std::unordered_set<bound_type> _cs;
    for (size_t _i = 0; _i != _s.size(); ++_i) {
        const char _b = _s[_i];
        if (_i + 2 < _s.size()) {
            const char _next = _s[_i + 1];
            const char _e = _s[_i + 2];
            if (_next == '-') {
                const auto& _r = _M_range(_b, _e);
                _cs.insert(_r.cbegin(), _r.cend());
                _i += 2;
                continue;
            }
        }
        _cs.insert(_b);
    }
    return std::string(_cs.cbegin(), _cs.cend());
}
auto random_object_impl<char>::_M_range(bound_type _b, bound_type _e) -> const std::string {
    const auto _bp = _M_alphabets(_b);
    const auto _ep = _M_alphabets(_e);
    if (_bp.first == nullptr || _ep.first == nullptr) return {};
    if (_bp.first == _ep.first) {
        if (_bp.second < _ep.second) {
            return std::string(_bp.first + _bp.second, _ep.first + _ep.second + 1);
        }
    }
    else {
        return std::string(_bp.first + _bp.second) + std::string(_ep.first, _ep.second + 1);
    }
    return {};
}
auto random_object_impl<char>::_M_alphabets(bound_type _x) -> std::pair<const char*, size_t> {
    if (isdigit(_x)) return std::make_pair(_digital_alphabets, _x - '0');
    if (islower(_x)) return std::make_pair(_lower_alphabets, _x - 'a');
    if (isupper(_x)) return std::make_pair(_upper_alphabets, _x - 'A');
    return std::make_pair<const char*, size_t>(nullptr, 0);
}



random_object_impl<std::string>::random_object_impl(uint_fast32_t _seed) : _vro(_seed), _bro(_seed) {}
void random_object_impl<std::string>::bound(bound_type _l, bound_type _u) {
    _bro.bound(_l, _u);
}
void random_object_impl<std::string>::bound(const std::string& _s) {
    _vro.bound(_s);
}
void random_object_impl<std::string>::bound(bound_type _l, bound_type _u, const std::string& _s) {
    _bro.bound(_l, _u);
    _vro.bound(_s);
}
void random_object_impl<std::string>::static_bound(bound_type _l, bound_type _u) {
    random_object<bound_type>::static_bound(_l, _u);
}
void random_object_impl<std::string>::static_bound(const std::string& _s) {
    random_object<value_type>::static_bound(_s);
}
void random_object_impl<std::string>::static_bound(bound_type _l, bound_type _u, const std::string& _s) {
    random_object<bound_type>::static_bound(_l, _u);
    random_object<value_type>::static_bound(_s);
}
void random_object_impl<std::string>::static_unbound() {
    random_object<bound_type>::static_unbound();
    random_object<value_type>::static_unbound();
}
auto random_object_impl<std::string>::rand(bound_type _l, bound_type _u, const std::string& _s) const -> obj_type {
    return _M_rand(_bro.rand(_l, _u), [this, _s]() {
        return this->_vro.rand(_s);
    });
}
auto random_object_impl<std::string>::rand(bound_type _l, bound_type _u) const -> obj_type {
    return _M_rand(_bro.rand(_l, _u), [this]() {
        return this->_vro.rand();
    });
}
auto random_object_impl<std::string>::rand() const -> obj_type {
    return _M_rand(_bro.rand(), [this]() {
        return this->_vro.rand();
    });
}
auto random_object_impl<std::string>::_M_rand(bound_type _length, std::function<value_type()>&& _gen) const -> obj_type {
    std::string _str(_length, 0);
    for (auto& _c : _str) {
        _c = _gen();
    }
    return _str;
}



random_object<void>::random_object(uint_fast32_t _seed) : _btro(_seed) {}

}