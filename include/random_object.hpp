#ifndef _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_
#define _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_

#include <cstddef>
#include <cassert>
#include <string>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <random>
#include <cstring>

namespace icy {

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



template <typename _Tp> struct random_object : public random_object_impl<typename std::remove_cv<_Tp>::type> {
    using base = random_object_impl<typename std::remove_cv<_Tp>::type>;
    random_object() = default;
    random_object(uint_fast32_t _seed) : base(_seed) {}
    virtual ~random_object() = default;
};

/**
 * @brief specialization for integral
 * @tparam _Tp integral type
*/
template <std::integral _Tp> struct random_object_impl<_Tp> {
protected:
    random_object_impl() : _rd(), _gen(_rd()) {}
    random_object_impl(uint_fast32_t _seed) : _gen(_seed) {}
    virtual ~random_object_impl() = default;
public:
    using obj_type = _Tp;
    using bound_type = obj_type;
    /**
     * @brief return random integral in [_l, _u)
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        std::uniform_int_distribution<_Tp> _dist(_l, _u - 1);
        return _dist(_gen);
    }
    /**
     * @brief return random integral
     */
    inline auto rand() const -> obj_type { return rand(lower_bound(), upper_bound()); }
    inline auto lower_bound() const -> bound_type { return (_s_global_bound ? _s_lb : _lb); }
    inline auto upper_bound() const -> bound_type { return (_s_global_bound ? _s_ub : _ub); }
    /**
     * @brief set lower and upper bound
     * @param _l lower bound
     * @param _u upper bound
     */
    void bound(bound_type _l, bound_type _u) { assert(_l < _u); _lb = _l; _ub = _u; }
    /**
     * @brief set global lower and upper bound
     * @param _l lower bound
     * @param _u upper bound
     */
    static void static_bound(bound_type _l, bound_type _u) { assert(_l < _u); _s_lb = _l; _s_ub = _u; _s_global_bound = true; }
    /**
     * @brief unset global lower and upper bound
     */
    static void static_unbound() { _s_global_bound = false; }
private:
    bound_type _lb = 0; // lower bound
    bound_type _ub = 2; // upper bound
    inline static bound_type _s_lb = 0; // static lower bound
    inline static bound_type _s_ub = 0; // static upper bound
    inline static bool _s_global_bound = false;
    std::random_device _rd;
    mutable std::mt19937 _gen;
};
/**
 * @brief specialization for floating point
 * @tparam _Tp floating point type
 */
template <std::floating_point _Tp> struct random_object_impl<_Tp> {
protected:
    random_object_impl() : _rd(), _gen(_rd()) {}
    random_object_impl(uint_fast32_t _seed) : _gen(_seed) {}
    virtual ~random_object_impl() = default;
public:
    using obj_type = _Tp;
    using bound_type = obj_type;
    /**
     * @brief return random floating point in [_l, _u)
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type {
        assert(_l < _u);
        std::uniform_real_distribution<_Tp> _dist(_l, _u);
        return _dist(_gen);
    }
    /**
     * @brief return random floating point
     */
    inline auto rand() const -> obj_type { return rand(lower_bound(), upper_bound()); }
    inline auto lower_bound() const -> bound_type { return (_s_global_bound ? _s_lb : _lb); }
    inline auto upper_bound() const -> bound_type { return (_s_global_bound ? _s_ub : _ub); }
    /**
     * @brief set lower and upper bound
     * @param _l lower bound
     * @param _u upper bound
     */
    inline void bound(bound_type _l, bound_type _u) { assert(_l < _u); _lb = _l; _ub = _u; }
    /**
     * @brief set global lower and upper bound
     * @param _l lower bound
     * @param _u upper bound
     */
    static void static_bound(bound_type _l, bound_type _u) { assert(_l < _u); _s_lb = _l; _s_ub = _u; _s_global_bound = true; }
    /**
     * @brief unset global lower and upper bound
     */
    static void static_unbound() { _s_global_bound = false; }
private:
    bound_type _lb = 0.0; // lower bound
    bound_type _ub = 1.0; // upper bound
    inline static bound_type _s_lb = 0.0; // static lower bound
    inline static bound_type _s_ub = 1.0; // static upper bound
    inline static bool _s_global_bound = false;
    std::random_device _rd;
    mutable std::mt19937 _gen;
};
/**
 * @brief specialization for bool (binomial distribution)
 */
template <> struct random_object_impl<bool> {
protected:
    random_object_impl();
    random_object_impl(uint_fast32_t _seed);
    virtual ~random_object_impl() = default;
public:
    using obj_type = unsigned;
    using bound_type = double;
    /**
     * @brief return random bool in binomial distribution
     * @param _n count
     * @param _p probability
     */
    auto rand(obj_type _n, bound_type _p = 0.5) const -> obj_type;
    /**
     * @brief return random bool
     */
    auto rand() const -> obj_type;
    auto count() const -> obj_type;
    auto probability() const -> bound_type;
    void bound(obj_type _n, bound_type _p);
    static void static_bound(obj_type _n, bound_type _p);
    static void static_unbound();
private:
    obj_type _n = 1u;
    bound_type _p = 0.5;
    inline static obj_type _s_n = 1u;
    inline static bound_type _s_p = 0.5;
    inline static bool _s_global_bound = false;
    std::random_device _rd;
    mutable std::mt19937 _gen;
};
/**
 * @brief specialization for char
 */
template <> struct random_object_impl<char> {
protected:
    random_object_impl();
    random_object_impl(uint_fast32_t _seed);
    virtual ~random_object_impl() = default;
public:
    using obj_type = char;
    using bound_type = char;
    /**
     * @brief set character bound
     * @param _s the collection of characters, support for range (such as "0-9a-f")
     */
    void bound(const std::string& _s);
    /**
     * @brief set character bound
     * @param _l from 0 to 9, from A to Z, from a to z
     * @param _u from 0 to 9, from A to Z, from a to z
     */
    void bound(bound_type _l, bound_type _u);
    /**
     * @brief set global character bound
     * @param _s the collection of characters, support for range (such as "0-9a-f")
     */
    static void static_bound(const std::string& _s);
    /**
     * @brief set global character bound
     * @param _l from 0 to 9, from A to Z, from a to z
     * @param _u from 0 to 9, from A to Z, from a to z
     */
    static void static_bound(bound_type _l, bound_type _u);
    /**
     * @brief unset global character bound
     */
    static void static_unbound();
    /**
     * @brief return the collection of characters
     */
    auto collection() const -> const std::string&;
    /**
     * @brief return random char in range `_s`
     * @param _s the collection of characters, support for range (such as "0-9a-f")
     */
    auto rand(const std::string& _s) const -> obj_type;
    /**
     * @brief return random char from `_l` to `_u`
     * @param _l from 0 to 9, from A to Z, from a to z
     * @param _u from 0 to 9, from A to Z, from a to z
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type;
    /**
     * @brief return random char
     */
    auto rand() const -> obj_type;
private:
    auto _M_rand_from_string(const std::string& _s) const -> obj_type;
private:
    static auto _M_range(const std::string& _s) -> const std::string;
    static auto _M_range(bound_type _b, bound_type _e) -> const std::string;
    static auto _M_alphabets(bound_type _c) -> std::pair<const char*, size_t>;
private:
    std::random_device _rd;
    mutable std::mt19937 _gen;
    std::string _collection = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    inline static std::string _s_collection = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    inline static bool _s_global_bound = false;
    static constexpr char _digital_alphabets[] = "0123456789";
    static constexpr char _upper_alphabets[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static constexpr char _lower_alphabets[] = "abcdefghijklmnopqrstuvwxyz";
};
/**
 * @brief specialization for std::string
 */
template <> struct random_object_impl<std::string> {
protected:
    random_object_impl() = default;
    random_object_impl(uint_fast32_t _seed);
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::string;
    using bound_type = std::string::size_type;
    using value_type = std::string::value_type;
    /**
     * @brief set length bound
     */
    void bound(bound_type _l, bound_type _u);
    /**
     * @brief set character range
     */
    void bound(const std::string& _s);
    /**
     * @brief set length bound and character range
     */
    void bound(bound_type _l, bound_type _u, const std::string& _s);
    /**
     * @brief set global length bound
     */
    static void static_bound(bound_type _l, bound_type _u);
    /**
     * @brief set global character range
     */
    static void static_bound(const std::string& _s);
    /**
     * @brief unset global length bound and character range
     */
    static void static_bound(bound_type _l, bound_type _u, const std::string& _s);
    static void static_unbound();
    /**
     * @brief return random string, with elements in [_l, _u] and size in [_ll, _ul)
     * @param _l lower bound (length)
     * @param _u upper bound (length)
     * @param _lv from 0 to 9, from A to Z, from a to z
     * @param _uv from 0 to 9, from A to Z, from a to z
     */
    auto rand(bound_type _l, bound_type _u, const std::string& _s) const -> obj_type;
    /**
     * @brief return random string, with size in [_l, _u)
     * @param _l lower length bound
     * @param _u upper length bound
     */
    auto rand(bound_type _l, bound_type _u) const -> obj_type;
    /**
     * @brief return random string
     */
    auto rand() const -> obj_type;
private:
    auto _M_rand(bound_type _length, std::function<value_type()>&& _gen) const -> obj_type;
private:
    random_object<value_type> _vro;
    random_object<bound_type> _bro;
};


template <typename _T1, typename _T2> struct random_object_impl<std::pair<_T1, _T2>> {
protected:
    random_object_impl() = default;
    random_object_impl(uint_fast32_t _seed) : _ro1(_seed), _ro2(_seed) {}
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::pair<_T1, _T2>;
    using bound_type = void;
    template <typename... _Ts1, typename... _Ts2> inline void
    bound(std::tuple<_Ts1...>&& _ts1, std::tuple<_Ts2...>&& _ts2) {
        _M_bound1(std::move(_ts1), std::make_index_sequence<sizeof...(_Ts1)>{});
        _M_bound2(std::move(_ts2), std::make_index_sequence<sizeof...(_Ts2)>{});
    }
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
    template <typename... _Ts1, size_t... _N1> requires (sizeof...(_Ts1) != 0) inline void
    _M_bound1(std::tuple<_Ts1...>&& _ts1, std::index_sequence<_N1...>) {
        _ro1.bound(std::forward<_Ts1>(std::get<_N1>(_ts1))...);
    };
    inline void _M_bound1(...) {}
    template <typename... _Ts2, size_t... _N2> requires (sizeof...(_Ts2) != 0) inline void
    _M_bound2(std::tuple<_Ts2...>&& _ts2, std::index_sequence<_N2...>) {
        _ro2.bound(std::forward<_Ts2>(std::get<_N2>(_ts2))...);
    };
    inline void _M_bound2(...) {}
private:
    random_object<_T1> _ro1;
    random_object<_T2> _ro2;
};

namespace {

template <size_t _Index, typename _This, typename... _Rest> struct random_tuple_object_impl;

template <size_t _Index, typename _This, typename... _Rest> struct random_tuple_object_impl : public random_tuple_object_impl<_Index + 1, _Rest...> {
protected:
    using base = random_tuple_object_impl<_Index + 1, _Rest...>;
    random_tuple_object_impl() = default;
    random_tuple_object_impl(uint_fast32_t _seed) : _ro(_seed), base(_seed) {}
    virtual ~random_tuple_object_impl() = default;
protected:
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
    template <typename... _Ts, typename... _Rests> inline void bound(std::tuple<_Ts...>&& _ts, _Rests&&... _rts) {
        base::bound(std::forward<_Rests>(_rts)...);
        _M_bound(std::move(_ts), std::make_index_sequence<sizeof...(_Ts)>{});
    }
    template <size_t _I, typename... _Bs> requires (_I == _Index) inline void bound(_Bs&&... _bs) {
        _ro.bound(std::forward<_Bs>(_bs)...);
    }
    template <size_t _I, typename... _Bs> requires (_I != _Index) inline void bound(_Bs&&... _bs) {
        if constexpr (_I > _Index) {
            base::template bound<_I>(std::forward<_Bs>(_bs)...);
        }
        else {
            throw std::out_of_range("random_object::bound<_I>");
        }
    }
private:
    template <typename... _Ts, size_t... _N> inline auto _M_rand(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) const -> _This {
        return _ro.rand(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
    template <typename... _Ts, size_t... _N> requires (sizeof...(_Ts) != 0) inline void _M_bound(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) {
        return _ro.bound(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
    inline void _M_bound(...) {}
protected:
    mutable std::tuple<_This, _Rest...> _tuple;
private:
    random_object<_This> _ro;
};
template <size_t _Index, typename _This> struct random_tuple_object_impl<_Index, _This> {
protected:
    random_tuple_object_impl() = default;
    random_tuple_object_impl(uint_fast32_t _seed) : _ro(_seed) {}
    virtual ~random_tuple_object_impl() = default;
public:
    template <typename... _Ts> inline auto make_tuple(std::tuple<_Ts...>&& _ts) const -> void {
        _tuple = std::make_tuple(_M_rand(std::move(_ts), std::make_index_sequence<sizeof...(_Ts)>{}));
    };
    virtual inline void make_tuple() const {
        _tuple = std::make_tuple(_ro.rand());
    }
    template <typename... _Ts> inline void bound(std::tuple<_Ts...>&& _ts) {
        _M_bound(std::move(_ts), std::make_index_sequence<sizeof...(_Ts)>{});
    }
    template <size_t _I, typename... _Bs> requires (_I == _Index) inline void bound(_Bs&&... _bs) {
        _ro.bound(std::forward<_Bs>(_bs)...);
    }
    template <size_t _I, typename... _Bs> requires (_I != _Index) inline void bound(_Bs&&... _bs) {
        throw std::out_of_range("random_object::bound<_I>");
    }
private:
    template <typename... _Ts, size_t... _N> inline auto _M_rand(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) const -> _This {
        return _ro.rand(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
    template <typename... _Ts, size_t... _N> requires (sizeof...(_Ts) != 0) inline void _M_bound(std::tuple<_Ts...>&& _ts, std::index_sequence<_N...>) {
        return _ro.bound(std::forward<_Ts>(std::get<_N>(_ts))...);
    }
    inline void _M_bound(...) {}
protected:
    mutable std::tuple<_This> _tuple;
private:
    random_object<_This> _ro;
};

}

template <typename... _Ts> struct random_object_impl<std::tuple<_Ts...>> : public random_tuple_object_impl<0, _Ts...> {
protected:
    using base = random_tuple_object_impl<0, _Ts...>;
    random_object_impl() = default;
    random_object_impl(uint_fast32_t _seed) : base(_seed) {}
    virtual ~random_object_impl() = default;
public:
    using obj_type = std::tuple<_Ts...>;
    using bound_type = void;
    template <typename... _Tts> auto rand(_Tts&&... _tts) const -> obj_type {
        base::make_tuple(std::forward<_Tts>(_tts)...);
        return base::_tuple;
    }
    /**
     * @brief set tuple element bound
     * @param _tts package of tuples, which contains arguments of elements' bound
     */
    template <typename... _Tts> void bound(_Tts&&... _tts) {
        base::bound(std::forward<_Tts>(_tts)...);
    }
    /**
     * @brief set tuple element bound
     * @tparam _I index of elements, which need to be constrained
     * @param _bs arguments of element's bound
     * @throw std::out_of_range if _I > sizeof(tuple<_Ts...>)
     */
    template <size_t _I, typename... _Bs> void bound(_Bs&&... _bs) {
        base::template bound<_I>(std::forward<_Bs>(_bs)...);
    }
    auto rand() const -> obj_type {
        base::make_tuple();
        return base::_tuple;
    };
};


/**
 * @brief random unsigned long generator for probability distribution
 */
template <> struct random_object<void> {
public:
    random_object() = default;
    random_object(uint_fast32_t _seed);
    virtual ~random_object() = default;
public:
    using object_type = size_t;
    using bound_type = double;
    /**
     * @brief return random unsigned long for probability distribution given in constructor
     */
    auto rand() const -> object_type {
        if (_distribution.empty()) return 0;
        return std::lower_bound(_distribution.cbegin(), _distribution.cend(), _btro.rand(0, _distribution.back())) - _distribution.cbegin();
    }
    /**
     * @brief update probability density
     * @tparam _Iter any iterator
     * @param _b begin
     * @param _e end
     */
    template <typename _Iter> void density(_Iter _b, _Iter _e) {
        _distribution.clear();
        for (_Iter _i = _b; _i != _e; ++_i) {
            _distribution.push_back(*_i + ((_distribution.empty() ? 0 : _distribution.back())));
        }
    }
    /**
     * @brief update probability density
     * @tparam _Iter any iterator
     * @param _b begin
     * @param _e end
     * @param _getter method to get value from iterator
     */
    template <typename _Iter> void density(_Iter _b, _Iter _e, std::function<bound_type(_Iter)> _getter) {
        _distribution.clear();
        for (_Iter _i = _b; _i != _e; ++_i) {
            _distribution.push_back(_getter(_i) + ((_distribution.empty() ? 0 : _distribution.back())));
        }
    }
    inline void density(std::initializer_list<bound_type> _il) {
        _distribution.clear();
        for (const auto& _i : _il) {
            _distribution.push_back(_i + ((_distribution.empty() ? 0 : _distribution.back())));
        }
    }
    const auto& distribution() const {
        return _distribution;
    }
private:
    random_object<bound_type> _btro;
    std::vector<bound_type> _distribution; // probability distribution
};


}

#endif // _ICY_CONTAINER_WRAPPER_RANDOM_OBJECT_HPP_