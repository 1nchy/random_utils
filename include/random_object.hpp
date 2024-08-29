#ifndef _ICY_CONTAINER_TEST_RANDOM_OBJECT_HPP_
#define _ICY_CONTAINER_TEST_RANDOM_OBJECT_HPP_

#include <cstddef>
#include <cassert>
#include <array>
#include <string>
#include <type_traits>
#include <initializer_list>

namespace icy {

namespace {
constexpr size_t _s_percentage_base = 100ul;
constexpr char _s_characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
}

template <typename _Tp> inline auto bound(const _Tp& _x, const _Tp& _b1, const _Tp& _b2) -> const _Tp& {
    return std::min(std::max(_x, std::min(_b1, _b2)), std::max(_b1, _b2));
}
inline size_t _S_diff_to_0(char _c) {
    if ('0' <= _c && _c <= '9') return _c - '0';
    else if ('A' <= _c && _c <= 'Z') return _c - 'A' + 10;
    else if ('a' <= _c && _c <= 'z') return _c - 'a' + 36;
    else return sizeof(_s_characters);
}
void _S_init_random_seed();

struct random_object_base;
template <typename _Tp> struct random_object;



struct random_object_base {
    random_object_base();
    virtual ~random_object_base() = default;
};
template <typename _Tp> struct random_object : public random_object_base {
    using obj_type = _Tp;
    using bound_type = _Tp;
    /**
     * @brief return random integral in [_l, _u)
     */
    template <typename _U = obj_type> requires std::is_integral<_U>::value auto
    rand(bound_type _l = 0, bound_type _u = 2) const -> obj_type {
        static_assert(std::is_same<_U, obj_type>::value);
        assert(_l < _u);
        return obj_type(std::rand() % (_u - _l) + _l);
    };
    /**
     * @brief return random floating point in [_l, _u)
     */
    template <typename _U = obj_type> requires std::is_floating_point<_U>::value auto
    rand(bound_type _l = 0.0, bound_type _u = 1.0) const -> obj_type {
        static_assert(std::is_same<_U, obj_type>::value);
        assert(_l < _u);
        return obj_type(static_cast<obj_type>(std::rand()) / RAND_MAX * (_u - _l) + _l);
    };
};
template <> struct random_object<char> : public random_object_base {
    using obj_type = char;
    using bound_type = char;
    /**
     * @brief return random char in [_l, _u]
     */
    auto rand(bound_type _l = '0', bound_type _u = 'z') const -> obj_type {
        assert(_l < _u);
        size_t _low = _S_diff_to_0(_l);
        size_t _up = _S_diff_to_0(_u);
        assert(0 <= _low && _up < sizeof(_s_characters));
        return _s_characters[_low + std::rand() % (_up - _low + 1)];
    };
};
template <> struct random_object<std::string> : public random_object_base {
    using obj_type = std::string;
    using bound_type = char;
    /**
     * @brief return random string, with elements in [_l, _u] and size in [_ll, _ul)
     */
    auto rand(size_t _ll = 3, size_t _ul = 4, bound_type _l = 'a', bound_type _u = 'z') const -> obj_type {
        assert(_l < _u);
        size_t _length = _ulro.rand(_ll, _ul);
        std::string _str(_length, 0);
        for (auto& _c : _str) {
            _c = _cro.rand(_l, _u);
        }
        return _str;
    };
private:
    random_object<size_t> _ulro;
    random_object<char> _cro;
};
template <> struct random_object<void> : public random_object_base {
    using object_type = size_t;
    using bound_type = double;
    /**
     * @brief return random unsigned for a given probability distribution
     */
    auto rand(std::initializer_list<bound_type> _il) -> object_type {
        object_type _ret = 0ul;
        bound_type _sum = 0;
        bound_type _r = _btro.rand(0, _s_percentage_base);
        for (auto _it = _il.begin(); _it != _il.end(); ++_it, ++_ret) {
            _sum += (*_it * _s_percentage_base);
            if (_sum > _r) {
                return _ret;
            }
        }
        return _ret;
    };
    /**
     * @brief return random unsigned for a given probability distribution
     */
    template <object_type _N> auto rand(const std::array<bound_type, _N>& _a) -> object_type {
        bound_type _sum = 0;
        bound_type _r = _btro.rand(0, _s_percentage_base);
        for (object_type _i = 0ul; _i != _N; ++_i) {
            _sum += (_a[_i] * _s_percentage_base);
            if (_sum > _r) {
                return _i;
            }
        }
        return _N;
    };
private:
    random_object<bound_type> _btro;
};


}

#endif // _ICY_CONTAINER_TEST_RANDOM_OBJECT_HPP_