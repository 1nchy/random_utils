#ifndef _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_
#define _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_

#include <cstddef>
#include <cassert>
#include <string>
#include <vector>
#include <type_traits>
#include <algorithm>

namespace icy {

namespace {
constexpr char _s_characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
inline size_t _S_diff_to_0(char _c) {
    if ('0' <= _c && _c <= '9') return _c - '0';
    else if ('A' <= _c && _c <= 'Z') return _c - 'A' + 10;
    else if ('a' <= _c && _c <= 'z') return _c - 'a' + 36;
    else return sizeof(_s_characters);
}
template <typename _Tp> inline auto bound(const _Tp& _x, const _Tp& _b1, const _Tp& _b2) -> const _Tp& {
    return std::min(std::max(_x, std::min(_b1, _b2)), std::max(_b1, _b2));
}
}

void _S_init_random_seed();

struct random_object_base;
/**
 * @brief random object implement (DO NOT USE DIRECTLY)
 * @tparam _Tp type without const and volatile
 */
template <typename _Tp> struct random_object_impl;
/**
 * @brief random object generator
 * @tparam _Tp random type (without reference)
 */
template <typename _Tp> struct random_object;



struct random_object_base {
    random_object_base();
protected:
    virtual ~random_object_base() = default;
};

template <typename _Tp> struct random_object : public random_object_impl<typename std::remove_cv<_Tp>::type> {
    using base = random_object_impl<typename std::remove_cv<_Tp>::type>;
    virtual ~random_object() = default;
};

/**
 * @brief specialization for integral
 * @tparam _Tp integral type
*/
template <std::integral _Tp> struct random_object_impl<_Tp> : public random_object_base {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = _Tp;
    using bound_type = obj_type;
    static inline bound_type lb = 0; // lower bound
    static inline bound_type ub = 2; // upper bound
    /**
     * @brief return random integral in [_l, _u)
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        return obj_type(std::rand() % (_u - _l) + _l);
    };
    /**
     * @brief return random integral
     */
    inline auto rand() const -> obj_type {
        return rand(lb, ub);
    };
};
/**
 * @brief specialization for floating point
 * @tparam _Tp floating point type
 */
template <std::floating_point _Tp> struct random_object_impl<_Tp> : public random_object_base {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = _Tp;
    using bound_type = obj_type;
    static inline bound_type lb = 0.0; // lower bound
    static inline bound_type ub = 1.0; // upper bound
    /**
     * @brief return random floating point in [_l, _u)
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        return obj_type(static_cast<obj_type>(std::rand()) / RAND_MAX * (_u - _l) + _l);
    };
    /**
     * @brief return random floating point
     */
    inline auto rand() const -> obj_type {
        return rand(lb, ub);
    };
};
/**
 * @brief specialization for char
 */
template <> struct random_object_impl<char> : public random_object_base {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = char;
    using bound_type = char;
    static inline bound_type lb = 'a'; // lower bound
    static inline bound_type ub = 'z'; // upper bound
    /**
     * @brief return random char in [_l, _u]
     * @param _l from 0 to 9, from A to Z, from a to z
     * @param _u from 0 to 9, from A to Z, from a to z
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        size_t _low = _S_diff_to_0(_l);
        size_t _up = _S_diff_to_0(_u);
        assert(0 <= _low && _up < sizeof(_s_characters));
        return _s_characters[_low + std::rand() % (_up - _low + 1)];
    };
    /**
     * @brief return random char
     */
    inline auto rand() const -> obj_type {
        return rand(lb, ub);
    };
};
/**
 * @brief specialization for std::string
 */
template <> struct random_object_impl<std::string> : public random_object_base {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::string;
    using bound_type = std::string::value_type;
    using size_type = std::string::size_type;
    static inline bound_type lb = 'a'; // lower bound
    static inline bound_type ub = 'z'; // upper bound
    static inline size_type llb = 3; // lower length bound
    static inline size_type ulb = 4; // upper length bound
    /**
     * @brief return random string, with elements in [_l, _u] and size in [_ll, _ul)
     * @param _ll lower length bound
     * @param _ul upper length bound
     * @param _l from 0 to 9, from A to Z, from a to z
     * @param _u from 0 to 9, from A to Z, from a to z
     */
    auto rand(size_type _ll, size_type _ul, bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        size_type _length = _ulro.rand(_ll, _ul);
        std::string _str(_length, 0);
        for (auto& _c : _str) {
            _c = _cro.rand(_l, _u);
        }
        return _str;
    };
    /**
     * @brief return random string, with size in [_ll, _ul)
     * @param _ll lower length bound
     * @param _ul upper length bound
     */
    inline auto rand(size_type _ll, size_type _ul) const -> obj_type {
        return rand(_ll, _ul, lb, ub);
    };
    /**
     * @brief return random string
     */
    inline auto rand() const -> obj_type {
        return rand(llb, ulb, lb, ub);
    };
private:
    random_object<size_type> _ulro;
    random_object<bound_type> _cro;
};


template <typename _T1, typename _T2> struct random_object_impl<std::pair<_T1, _T2>> : public random_object_base {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::pair<_T1, _T2>;
    using bound_type = void;
    template <typename... _Ts1, typename... _Ts2> inline auto
    rand(std::tuple<_Ts1...>&& _ts1, std::tuple<_Ts2...>&& _ts2) const -> obj_type {
        return _M_rand(std::move(_ts1), std::make_index_sequence<sizeof...(_Ts1)>{}, std::move(_ts2), std::make_index_sequence<sizeof...(_Ts2)>{});
    };
    auto rand() const -> obj_type {
        return std::make_pair(_ro1.rand(), _ro2.rand());
    };
private:
    template <typename... _Ts1, size_t... _N1, typename... _Ts2, size_t... _N2> auto
    _M_rand(
        std::tuple<_Ts1...>&& _ts1, std::index_sequence<_N1...>,
        std::tuple<_Ts2...>&& _ts2, std::index_sequence<_N2...>
    ) const -> obj_type {
        return std::make_pair(
            _ro1.rand(std::forward<_Ts1>(std::get<_N1>(_ts1))...),
            _ro2.rand(std::forward<_Ts2>(std::get<_N2>(_ts2))...)
        );
    };
private:
    random_object<_T1> _ro1;
    random_object<_T2> _ro2;
};

namespace {

template <size_t _Index, typename _This, typename... _Rest> struct random_tuple_object_impl;

template <size_t _Index, typename _This, typename... _Rest> struct random_tuple_object_impl : public random_tuple_object_impl<_Index + 1, _Rest...> {
protected:
    virtual ~random_tuple_object_impl() = default;
public:
    using base = random_tuple_object_impl<_Index + 1, _Rest...>;
    template <typename... _Ts, typename... _Rests> inline auto make_tuple(std::tuple<_Ts...>&& _ts, _Rests&&... _rts) const -> void {
        base::make_tuple(std::forward<_Rests>(_rts)...);
        _tuple = std::tuple_cat(
            std::forward_as_tuple(_M_rand(std::move(_ts), std::make_index_sequence<sizeof...(_Ts)>{})),
            base::_tuple
        );
    };
    inline void make_tuple() const override {
        base::make_tuple();
        _tuple = std::tuple_cat(std::forward_as_tuple(_ro.rand()), base::_tuple);
    }
private:
    template <typename... _Ts, size_t... _N> inline auto _M_rand(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) const -> _This {
        return _ro.rand(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
protected:
    mutable std::tuple<_This, _Rest...> _tuple;
private:
    random_object<_This> _ro;
};
template <size_t _Index, typename _This> struct random_tuple_object_impl<_Index, _This> : public random_object_base {
protected:
    virtual ~random_tuple_object_impl() = default;
public:
    template <typename... _Ts> inline auto make_tuple(std::tuple<_Ts...>&& _ts) const -> void {
        _tuple = std::make_tuple(_M_rand(std::move(_ts), std::make_index_sequence<sizeof...(_Ts)>{}));
    };
    virtual inline void make_tuple() const {
        _tuple = std::make_tuple(_ro.rand());
    }
private:
    template <typename... _Ts, size_t... _N> inline auto _M_rand(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) const -> _This {
        return _ro.rand(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
protected:
    mutable std::tuple<_This> _tuple;
private:
    random_object<_This> _ro;
};

}

template <typename... _Ts> struct random_object_impl<std::tuple<_Ts...>> : public random_tuple_object_impl<0, _Ts...> {
protected:
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::tuple<_Ts...>;
    using bound_type = void;
    using base = random_tuple_object_impl<0, _Ts...>;
    template <typename... _Tts> auto rand(_Tts&&... _tts) const -> obj_type {
        base::make_tuple(std::forward<_Tts>(_tts)...);
        return base::_tuple;
    }
    auto rand() const -> obj_type {
        base::make_tuple();
        return base::_tuple;
    };
};


/**
 * @brief random unsigned long generator for probability distribution
 */
template <> struct random_object<void> : public random_object_base {
    using object_type = size_t;
    using bound_type = double;
    virtual ~random_object() = default;
    random_object() = default;
    random_object(std::initializer_list<bound_type> _il) {
        update_density(_il);
    }
    /**
     * @brief return random unsigned long for probability distribution given in constructor
     */
    auto rand() const -> object_type {
        if (_density.empty()) return 0;
        return std::lower_bound(_density.cbegin(), _density.cend(), _btro.rand(0, _density.back())) - _density.cbegin();
    }
    /**
     * @brief update probability density
     * @tparam _Iter any iterator
     * @param _b begin
     * @param _e end
     * @param _getter method to get value from iterator, operator*(_Iter)->bound_type by default
     */
    template <typename _Iter> void update_density(_Iter _b, _Iter _e, auto _getter = [](_Iter _i) -> bound_type { return *_i; }) {
        _density.clear();
        for (_Iter _i = _b; _i != _e; ++_i) {
            _density.push_back(_getter(_i) + ((_density.empty() ? 0 : _density.back())));
        }
    }
    inline void update_density(std::initializer_list<bound_type> _il) {
        _density.clear();
        for (const auto& _i : _il) {
            _density.push_back(_i + ((_density.empty() ? 0 : _density.back())));
        }
    }
private:
    random_object<bound_type> _btro;
    std::vector<bound_type> _density; // probability density
};


}

#endif // _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_