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

template <typename... _Ts> std::ostream& operator<<(std::ostream& _os, const std::tuple<_Ts...>& _tuple);
template <typename _T1, typename _T2> std::ostream& operator<<(std::ostream& _os, const std::pair<_T1, _T2>& _pair);

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
template <typename _T1, typename _T2> std::ostream& operator<<(std::ostream& _os, const std::pair<_T1, _T2>& _pair) {
    _os << '(' << _pair.first << ", " << _pair.second << ')';
    return _os;
}

struct virtual_callable;
template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable;
template <typename _Container> struct callable_constructor;
// template <typename _Container> struct callable_assignment;
struct virtual_callable {
    virtual void operator()() = 0;
    virtual ~virtual_callable() = default;
    virtual std::string message() const = 0;
};



namespace {

template <size_t _Index, typename _This, typename... _Rest> struct callable_impl;

template <size_t _Index, typename _This, typename... _Rest> struct callable_impl : public callable_impl<_Index + 1, _Rest...> {
    using base = callable_impl<_Index + 1, _Rest...>;
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
    using tuple_type = tuple_cat_result<value_type, typename base::tuple_type>::type;
    virtual ~callable_impl() override = default;
    void operator()() override {
        base::operator()();
        _tuple = std::tuple_cat(std::forward_as_tuple(_ro.rand()), base::_tuple);
    }
protected:
    tuple_type _tuple;
private:
    random_object<value_type> _ro;
};
template <size_t _Index, typename _This> struct callable_impl<_Index, _This> : public virtual_callable {
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
    using tuple_type = std::tuple<value_type>;
    virtual ~callable_impl() override = default;
    void operator()() override {
        _tuple = std::make_tuple(_ro.rand());
    }
protected:
    tuple_type _tuple;
private:
    random_object<value_type> _ro;
};

}

template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable : public callable_impl<0, _Args...> {
    using base = callable_impl<0, _Args...>;
    using container_type = _Container;
    using container_pointer = container_type*;
    using method_type = std::conditional<_Const, _R(container_type::*)(_Args...)const, _R(container_type::*)(_Args...)>::type;
    callable(container_pointer& _c, method_type _p) : _container(_c), _call(_p) {}
    virtual ~callable() override = default;
    void operator()() override {
        base::operator()();
        // std::cout << base::_tuple << std::endl;
        /**
         * @details std::apply(_call, std::tuple_cat(std::forward_as_tuple(*_container), base::_tuple));
         * std::apply cant handle reference parameter.
         */
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
    container_pointer& _container;
    const method_type _call;
};
template <typename _Container, bool _Const, typename _R> struct callable<_Container, _Const, _R> : public virtual_callable {
    using container_type = _Container;
    using container_pointer = container_type*;
    using method_type = std::conditional<_Const, _R(container_type::*)()const, _R(container_type::*)()>::type;
    callable(container_pointer& _c, method_type _p) : _container(_c), _call(_p) {}
    virtual ~callable() override = default;
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
    container_pointer& _container;
    const method_type _call;
};

template <typename _Container> struct callable_constructor : public virtual_callable {
    using container_type = _Container;
    using container_pointer = container_type*;
    callable_constructor(container_pointer& _c, std::function<void(void)>&& _f) : _container(_c), _func(_f) {}
    virtual ~callable_constructor() override = default;
    void operator()() override {
        _func();
    }
    std::string message() const override {
        return "";
    }
private:
    container_pointer& _container;
    std::function<void(void)> _func;
};
template <typename _Container> struct callable_assignment : public virtual_callable {
    using container_type = _Container;
    using container_pointer = container_type*;
    callable_assignment(container_pointer& _c, std::function<void(void)>&& _f) : _container(_c), _func(_f) {}
    virtual ~callable_assignment() override = default;
    void operator()() override {
        _func();
    }
    std::string message() const override {
        return "";
    }
private:
    container_pointer& _container;
    std::function<void(void)> _func;
};



template <typename _Tp> class wrapper;
template <typename _Tp> struct wrapper_alloc;

template <typename _Tp> struct wrapper_alloc : public std::allocator<_Tp> {
    using container_type = _Tp;
    typedef std::allocator<_Tp> container_allocator_type;
    typedef std::allocator_traits<container_allocator_type> container_alloc_traits;

    container_allocator_type& _M_get_container_allocator() { return *static_cast<container_allocator_type*>(this); }
    const container_allocator_type& _M_get_container_allocator() const { return *static_cast<const container_allocator_type*>(this); }

    template <typename... _Args> container_type* _M_allocate_container(_Args&&... _args) const {
        container_allocator_type _a = _M_get_container_allocator();
        auto _ptr = container_alloc_traits::allocate(_a, 1);
        container_type* _p = std::addressof(*_ptr);
        container_alloc_traits::construct(_a, _p, std::forward<_Args>(_args)...);
        return _p;
    }
    void _M_deallocate_container(container_type* _p) const {
        container_allocator_type _a = _M_get_container_allocator();
        container_alloc_traits::destroy(_a, _p);
        container_alloc_traits::deallocate(_a, _p, 1);
    }
};

/**
 * @brief container and method wrapper
 * @tparam _Tp container type
 */
template <typename _Tp> class wrapper : public wrapper_alloc<_Tp> {
    using self = wrapper<_Tp>;
    using container_type = _Tp;
    enum { _operation_n = 1000000ul };
public:
    wrapper();
    wrapper(const self&) = delete;
    self& operator=(const self&) = delete;
    ~wrapper();
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

    auto enroll_copy_construtor(double _weight = 1.0) -> void;
    auto enroll_move_construtor(double _weight = 1.0) -> void;
    auto enroll_copy_assignment(double _weight = 1.0) -> void;
    auto enroll_move_assignment(double _weight = 1.0) -> void;
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
    /**
     * @brief call an enrolled method with arguments generated randomly
     * @param _k string key of the method
     */
    auto _M_call_with_random_args(const std::string& _k) -> void;
    void _M_call_constructor();
    void _M_call_assignment_operator(const size_t _n);
    inline auto _M_get_message(const std::string& _k) const -> std::string {
        if (_enrollment.contains(_k)) {
            return _enrollment.at(_k)->message();
        }
        else if (_object_oriented_methods.contains(_k)) {
            return _object_oriented_methods.at(_k)->message();
        }
        return "";
    }
private:
    template <typename _Container, std::enable_if<has_unsigned_check_const<_Container>::value, unsigned>::type = 0u> 
    unsigned _M_check(const _Container* _c) const { return _c->check(); }
    template <typename _Container> unsigned _M_check(...) const { return 0u; }
private:
    bool _log_enabled = false;
    std::ofstream _fs;
    container_type* _container;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _enrollment;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _object_oriented_methods;
    std::vector<std::pair<const double, const std::string>> _dist; // the weight distribution of methods
    random_object<void> _vro;
};


template <typename _Tp> wrapper<_Tp>::wrapper() {
    _container = this->_M_allocate_container();
    // this->_M_construct_container(_container);
};
template <typename _Tp> wrapper<_Tp>::~wrapper() {
    // this->_M_destruct_container(_container);
    this->_M_deallocate_container(_container);
};


template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight) -> void {
    auto _ptr = std::make_shared<callable<container_type, false, _R, _Args...>>(std::ref(_container), _p);
    _enrollment[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight) -> void {
    auto _ptr = std::make_shared<callable<container_type, true, _R, _Args...>>(std::ref(_container), _p);
    _enrollment[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};

template <typename _Tp> auto wrapper<_Tp>::enroll_copy_construtor(double _weight) -> void {
    assert(std::is_copy_constructible<container_type>::value);
    const std::string _k = "copy constructor";
    auto _ptr = std::make_shared<callable_constructor<container_type>>(std::ref(_container), [this]() {
        this->_M_call_constructor();
    });
    _object_oriented_methods[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> auto wrapper<_Tp>::enroll_move_construtor(double _weight) -> void {
    assert(std::is_move_constructible<container_type>::value);
};
template <typename _Tp> auto wrapper<_Tp>::enroll_copy_assignment(double _weight) -> void {
    assert(std::is_copy_assignable<container_type>::value);
    // const std::string _k = "copy assignment";
    // auto _ptr = std::make_shared<callable_assignment<container_type>>(std::ref(_container));
    // _object_oriented_methods[_k] = _ptr;
    // _dist.emplace_back(_weight, _k);
};
template <typename _Tp> auto wrapper<_Tp>::enroll_move_assignment(double _weight) -> void {
    assert(std::is_move_assignable<container_type>::value);
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
        _M_call_with_random_args(_k);
        if (_log_enabled)
            _fs << _k << _M_get_message(_k) << std::endl;
        if (const auto _result = try_check()) {
            if (_log_enabled)
                std::cout << _k << _M_get_message(_k) << std::endl;
            throw std::logic_error(std::string("logic error inside the container : " + std::to_string(_result)));
        }
    }
};
template <typename _Tp> auto wrapper<_Tp>::try_check() const -> unsigned {
    return _M_check<container_type>(_container);
};
template <typename _Tp> auto wrapper<_Tp>::log_file(const std::string& _file) -> void {
    _fs = std::ofstream(_file, std::ios_base::out | std::ios_base::trunc);
};


template <typename _Tp> auto wrapper<_Tp>::_M_call_with_random_args(const std::string& _k) -> void {
    if (_enrollment.contains(_k)) {
        _enrollment.at(_k)->operator()();
    }
    else if (_object_oriented_methods.contains(_k)) {
        _object_oriented_methods.at(_k)->operator()();
    }
};
template <typename _Tp> auto wrapper<_Tp>::_M_call_constructor() -> void {
    container_type* _nc = this->_M_allocate_container(*_container);
    this->_M_deallocate_container(_container);
    _container = _nc;
};
template <typename _Tp> auto wrapper<_Tp>::_M_call_assignment_operator(const size_t _n) -> void {
    container_type* _nc = this->_M_allocate_container();
    container_type* _oc = _container;
    _container = _nc;
    for (size_t _i = 0; _i < _n; ++_i) {
        const std::string& _k = _dist.at(_vro.rand()).second;
        _M_call_with_random_args(_k);
        if (const auto _result = try_check()) {
            throw std::logic_error(std::string("logic error inside the container : " + std::to_string(_result)));
        }
    }
    _container = _oc;
    *_container = *_nc;
    this->_M_deallocate_container(_nc);
};

}

}

#endif // _ICY_CONTAINER_WRAPPER_HPP_