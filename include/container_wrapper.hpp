#ifndef _ICY_CONTAINER_WRAPPER_HPP_
#define _ICY_CONTAINER_WRAPPER_HPP_

#include <cstddef>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "random_object.hpp"

namespace icy {

namespace container {

namespace {
template <size_t _Index, typename _Tuple> struct tuple_print {
    static void print(std::ostream& _os, const _Tuple& _t) {
        tuple_print<_Index - 1, _Tuple>::print(_os, _t);
        _os << ", " << std::get<_Index - 1>(_t);
    }
};
template <typename _Tuple> struct tuple_print<1, _Tuple> {
    static void print(std::ostream& _os, const _Tuple& _t) {
        _os << std::get<0>(_t);
    }
};
template <typename... _Ts> struct tuple_cat_result;
template <> struct tuple_cat_result<> {
    using type = std::tuple<>;
};
template <typename... _Ts> struct tuple_cat_result<std::tuple<_Ts...>> {
    using type = std::tuple<_Ts...>;
};
template <typename _Tp, typename... _Rs> struct tuple_cat_result<_Tp, std::tuple<_Rs...>> {
    using type = std::tuple<_Tp, _Rs...>;
};
template <typename... _Ls, typename _Tp> struct tuple_cat_result<std::tuple<_Ls...>, _Tp> {
    using type = std::tuple<_Ls..., _Tp>;
};
template <typename... _Ls, typename... _Rs> struct tuple_cat_result<std::tuple<_Ls...>, std::tuple<_Rs...>> {
    using type = std::tuple<_Ls..., _Rs...>;
};

template <typename _Container> struct has_unsigned_check_const {
private:
    // template <typename _U> static auto _M_check(int)
    //  -> decltype(std::declval<_U>().check(), std::true_type());
    template <typename _U, unsigned (_U::*)() const> struct unsigned_check_const {};
    template <typename _U> static auto _M_check(unsigned_check_const<_U, &_U::check>*) -> std::true_type;
    template <typename _U> static auto _M_check(...) -> std::false_type;
public:
    enum { value = std::is_same<decltype(_M_check<_Container>(0)), std::true_type>::value };
};
}
template <typename... _Ts> std::ostream& operator<<(std::ostream& _os, const std::tuple<_Ts...>& _tuple) {
    _os << '(';
    tuple_print<sizeof...(_Ts), decltype(_tuple)>::print(_os, _tuple);
    _os << ')';
    return _os;
}

struct virtual_callable;
template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable;
template <size_t _Index, typename _This, typename... _Rest> struct callable_impl;
struct virtual_callable {
    virtual void operator()() = 0;
    virtual ~virtual_callable() = default;
    virtual std::string message() const = 0;
};


template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable : public callable_impl<0, _Args...> {
    using base = callable_impl<0, _Args...>;
    using container_type = _Container;
    using method_type = std::conditional<_Const, _R(container_type::*)(_Args...)const, _R(container_type::*)(_Args...)>::type;
    callable(container_type* _c, method_type _p) : _container(_c), _call(_p) {}
    void operator()() override {
        base::operator()();
        // std::cout << base::_tuple << std::endl;
        invoke_tuple(base::_tuple);
    }
    std::string message() const override {
        std::stringstream _ss;
        _ss << base::_tuple;
        return _ss.str();
    }
    _R invoke_with_result(_Args&&... _args) {
        return (_container->*_call)(std::forward<_Args>(_args)...);
    }
private:
    template <typename _Tuple> void invoke_tuple(_Tuple& _tuple) {
        _M_invoke_tuple(_tuple, std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <typename _Tuple, size_t... _N> void _M_invoke_tuple(_Tuple& _tuple, std::index_sequence<_N...>) {
        // (_container->*_call)(std::get<_N>(_tuple)...);
        (_container->*_call)(std::forward<_Args>(std::get<_N>(_tuple))...);
    }
private:
    container_type* const _container;
    const method_type _call;
};
template <typename _Container, bool _Const, typename _R> struct callable<_Container, _Const, _R> : public virtual_callable {
    using container_type = _Container;
    using method_type = std::conditional<_Const, _R(container_type::*)()const, _R(container_type::*)()>::type;
    callable(container_type* _c, method_type _p) : _container(_c), _call(_p) {}
    void operator()() override {
        (_container->*_call)();
    }
    std::string message() const override {
        return "";
    }
    _R invoke_with_result() {
        return (_container->*_call)();
    }
private:
    container_type* const _container;
    const method_type _call;
};

template <size_t _Index, typename _This, typename... _Rest> struct callable_impl : public callable_impl<_Index + 1, _Rest...> {
    using base = callable_impl<_Index + 1, _Rest...>;
    ~callable_impl() override = default;
    void operator()() override {
        base::operator()();
        _tuple = std::tuple_cat(std::make_tuple(_ro.rand()), base::_tuple);
    }
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
    typename tuple_cat_result<value_type, decltype(base::_tuple)>::type _tuple;
    // typename std::__tuple_cat_result<std::tuple<value_type>, decltype(base::_tuple)>::__type _tuple;
private:
    random_object<value_type> _ro;
};
template <size_t _Index, typename _This> struct callable_impl<_Index, _This> : public virtual_callable {
    ~callable_impl() override = default;
    void operator()() override {
        _tuple = std::make_tuple(_ro.rand());
    }
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
    std::tuple<value_type> _tuple;
private:
    random_object<value_type> _ro;
};


/**
 * @brief container and method wrapper
 * @tparam _Tp container type
 */
template <typename _Tp> class wrapper {
    using container_type = _Tp;
    enum { _operation_n = 1000000ul };
public:
    /**
     * @brief enroll string key, method pointer and probability
     * @tparam _R return type of the method
     * @tparam _Args arguments type of the method
     * @param _k string key
     * @param _p method pointer
     * @param _weight the weight of method invocation (default 1.0)
     */
    template <typename _R, typename... _Args> auto
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight = 1.0) -> void;
    template <typename _R, typename... _Args> auto
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight = 1.0) -> void;
    /**
     * @brief call an enrolled method with arguments generated randomly
     * @param _k string key of the method
     */
    auto call_with_random_args(const std::string& _k) -> void;
    /**
     * @brief call an enrolled method
     * @tparam _R return type of the specific method
     * @tparam _Args arguments type of the specific method
     * @param _k string key
     * @param _args arguments of the method
     * @return the result of the method
     * @throw std::invalid_argument
     */
    template <typename _R, typename... _Args> auto
    call(const std::string& _k, _Args&&... _args) -> _R;
    /**
     * @brief random operation on the container specific times
     * @param _operation_n the number of operations
     * @throw std::logic_error
     */
    void run(const size_t _n = _operation_n);
    /**
     * @brief try to call check method
     * @return 0 if no method, otherwise the checking result
     */
    unsigned try_check() const;
    void log_file(const std::string& _file);
    void enable_log() { _log_enabled = true; }
    void disable_log() { _log_enabled = false; }
private:
    template <typename _Container, std::enable_if<has_unsigned_check_const<_Container>::value, unsigned>::type = 0u> 
    unsigned _M_check(const _Container* _c) const { return _c->check(); }
    template <typename _Container> unsigned _M_check(...) const { return 0u; }
private:
    bool _log_enabled = false;
    std::ofstream _fs;
    container_type _container;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _enrollment;
    std::vector<std::pair<const double, const std::string>> _dist; // the weight distribution of methods
    random_object<void> _vro;
};


template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight) -> void {
    auto _ptr = std::make_shared<callable<container_type, false, _R, _Args...>>(&_container, _p);
    _enrollment[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight) -> void {
    auto _ptr = std::make_shared<callable<container_type, true, _R, _Args...>>(&_container, _p);
    _enrollment[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> auto wrapper<_Tp>::call_with_random_args(const std::string& _k) -> void {
    _enrollment.at(_k)->operator()();
};
template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
call(const std::string& _k, _Args&&... _args) -> _R {
    auto* _const_callable_ptr = dynamic_cast<callable<container_type, true, _R, _Args...>*>(_enrollment.at(_k).get());
    if (_const_callable_ptr != nullptr) {
        return _const_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    auto* _nonconst_callable_ptr = dynamic_cast<callable<container_type, false, _R, _Args...>*>(_enrollment.at(_k).get());
    if (_nonconst_callable_ptr != nullptr) {
        return _nonconst_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    throw std::invalid_argument("_R or sizeof...(_Args)");
};
template <typename _Tp> auto wrapper<_Tp>::run(const size_t _n) -> void {
    _vro.update_density(_dist.cbegin(), _dist.cend(), [](decltype(_dist)::const_iterator _i) {
        return _i->first;
    });
    for (size_t _i = 0; _i != _n; ++_i) {
        const std::string& _k = _dist.at(_vro.rand()).second;
        call_with_random_args(_k);
        if (_log_enabled)
            _fs << _k << _enrollment.at(_k)->message() << std::endl;
        if (const auto _result = try_check() != 0u) {
            if (_log_enabled)
                std::cout << _k << _enrollment.at(_k)->message() << std::endl;
            throw std::logic_error(std::string("logic error inside the container : " + std::to_string(_result)));
        }
    }
};
template <typename _Tp> auto wrapper<_Tp>::try_check() const -> unsigned {
    return _M_check<container_type>(std::addressof(_container));
};
template <typename _Tp> auto wrapper<_Tp>::log_file(const std::string& _file) -> void {
    _fs = std::ofstream(_file, std::ios_base::out | std::ios_base::trunc);
};

}

}

#endif // _ICY_CONTAINER_WRAPPER_HPP_