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
struct virtual_callable {
    virtual void operator()() = 0;
    virtual ~virtual_callable() = default;
    virtual std::string arguments() const = 0;
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
    using obj_type = _Container;
    using container_pointer = obj_type*;
    using method_type = std::conditional<_Const, _R(obj_type::*)(_Args...)const, _R(obj_type::*)(_Args...)>::type;
    callable(container_pointer& _c, method_type _p) : _object(_c), _call(_p) {}
    virtual ~callable() override = default;
    void operator()() override {
        base::operator()();
        /**
         * @details std::apply(_call, std::tuple_cat(std::forward_as_tuple(*_object), base::_tuple));
         * std::apply can't handle reference parameters.
         */
        invoke_tuple(base::_tuple);
    }
    std::string arguments() const override {
        std::ostringstream _ss;
        _ss << base::_tuple;
        return _ss.str();
    }
    _R invoke_with_result(_Args&&... _args) {
        return (_object->*_call)(std::forward<_Args>(_args)...);
    }
private:
    template <typename _Tuple> void invoke_tuple(_Tuple& _tuple) {
        _M_invoke_tuple(_tuple, std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <typename _Tuple, size_t... _N> void _M_invoke_tuple(_Tuple& _tuple, std::index_sequence<_N...>) {
        (_object->*_call)(std::forward<_Args>(std::get<_N>(_tuple))...);
    }
private:
    container_pointer& _object;
    const method_type _call;
};
template <typename _Container, bool _Const, typename _R> struct callable<_Container, _Const, _R> : public virtual_callable {
    using obj_type = _Container;
    using container_pointer = obj_type*;
    using method_type = std::conditional<_Const, _R(obj_type::*)()const, _R(obj_type::*)()>::type;
    callable(container_pointer& _c, method_type _p) : _object(_c), _call(_p) {}
    virtual ~callable() override = default;
    void operator()() override {
        (_object->*_call)();
    }
    std::string arguments() const override {
        return "";
    }
    _R invoke_with_result() {
        return (_object->*_call)();
    }
private:
    container_pointer& _object;
    const method_type _call;
};

template <typename _Container> struct callable_constructor : public virtual_callable {
    using obj_type = _Container;
    using container_pointer = obj_type*;
    callable_constructor(container_pointer& _c, std::function<void(void)>&& _f) : _object(_c), _func(_f) {}
    virtual ~callable_constructor() override = default;
    void operator()() override {
        _func();
    }
    std::string arguments() const override {
        return "";
    }
private:
    container_pointer& _object;
    std::function<void(void)> _func;
};



template <typename _Tp> class random_caller;
template <typename _Tp> struct random_caller_alloc;

template <typename _Tp> struct random_caller_alloc : public std::allocator<_Tp> {
    using obj_type = _Tp;
    typedef std::allocator<_Tp> container_allocator_type;
    typedef std::allocator_traits<container_allocator_type> container_alloc_traits;

    container_allocator_type& _M_get_container_allocator() { return *static_cast<container_allocator_type*>(this); }
    const container_allocator_type& _M_get_container_allocator() const { return *static_cast<const container_allocator_type*>(this); }

    template <typename... _Args> obj_type* _M_allocate_container(_Args&&... _args) const {
        container_allocator_type _a = _M_get_container_allocator();
        auto _ptr = container_alloc_traits::allocate(_a, 1);
        obj_type* _p = std::addressof(*_ptr);
        container_alloc_traits::construct(_a, _p, std::forward<_Args>(_args)...);
        return _p;
    }
    void _M_deallocate_container(obj_type* _p) const {
        container_allocator_type _a = _M_get_container_allocator();
        container_alloc_traits::destroy(_a, _p);
        container_alloc_traits::deallocate(_a, _p, 1);
    }
};

/**
 * @brief container and method caller
 * @tparam _Tp container type
 */
template <typename _Tp> class random_caller : public random_caller_alloc<_Tp> {
    using self = random_caller<_Tp>;
    using obj_type = _Tp;
    enum { operation_n = 1000000ul };
public:
    random_caller();
    random_caller(const self&) = delete;
    self& operator=(const self&) = delete;
    ~random_caller();
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
    template <typename _R, typename... _Args> auto
    push_callback(_R(_Tp::*_p)(_Args...)) -> void;
    template <typename _R, typename... _Args> auto
    push_callback(_R(_Tp::*_p)(_Args...)const) -> void;

    auto enroll_copy_construtor(double _weight = 1.0) -> void;
    auto enroll_move_construtor(double _weight = 1.0) -> void;
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
     */
    bool run(const size_t _n = operation_n);
    const auto& get_commands() const { return _commands; }
    const auto& get_arguments() const { return _arguments; }
    const auto& get_exception() const { return _exception; }
private:
    void _M_call_copy_constructor();
    void _M_call_move_constructor();
    void _M_save_commands_and_arguments(const std::string& _k);
private:
    obj_type* _object;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _methods;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _constructors;
    std::vector<std::shared_ptr<virtual_callable>> _callbacks;
    std::vector<std::pair<const double, const std::string>> _dist; // the weight distribution of methods
    random_object<void> _vro;
    std::vector<std::string> _commands;
    std::vector<std::string> _arguments;
    std::string _exception;
};


template <typename _Tp> random_caller<_Tp>::random_caller() {
    _object = this->_M_allocate_container();
};
template <typename _Tp> random_caller<_Tp>::~random_caller() {
    this->_M_deallocate_container(_object);
};


template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight) -> void {
    if (_constructors.contains(_k)) return;
    auto _ptr = std::make_shared<callable<obj_type, false, _R, _Args...>>(std::ref(_object), _p);
    _methods[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight) -> void {
    if (_constructors.contains(_k)) return;
    auto _ptr = std::make_shared<callable<obj_type, true, _R, _Args...>>(std::ref(_object), _p);
    _methods[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
push_callback(_R(_Tp::*_p)(_Args...)) -> void {
    auto _ptr = std::make_shared<callable<obj_type, false, _R, _Args...>>(std::ref(_object), _p);
    _callbacks.emplace_back(_ptr);
}
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
push_callback(_R(_Tp::*_p)(_Args...)const) -> void {
    auto _ptr = std::make_shared<callable<obj_type, true, _R, _Args...>>(std::ref(_object), _p);
    _callbacks.emplace_back(_ptr);
}

template <typename _Tp> auto random_caller<_Tp>::enroll_copy_construtor(double _weight) -> void {
    assert(std::is_copy_constructible<obj_type>::value);
    const std::string _k = "copy constructor";
    auto _ptr = std::make_shared<callable_constructor<obj_type>>(std::ref(_object), [this]() {
        this->_M_call_copy_constructor();
    });
    _constructors[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};
template <typename _Tp> auto random_caller<_Tp>::enroll_move_construtor(double _weight) -> void {
    assert(std::is_move_constructible<obj_type>::value);
    const std::string _k = "move constructor";
    auto _ptr = std::make_shared<callable_constructor<obj_type>>(std::ref(_object), [this]() {
        this->_M_call_move_constructor();
    });
    _constructors[_k] = _ptr;
    _dist.emplace_back(_weight, _k);
};

template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
call(const std::string& _k, _Args&&... _args) -> _R {
    auto* _const_callable_ptr = dynamic_cast<callable<obj_type, true, _R, _Args...>*>(_methods.at(_k).get());
    if (_const_callable_ptr != nullptr) {
        return _const_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    auto* _nonconst_callable_ptr = dynamic_cast<callable<obj_type, false, _R, _Args...>*>(_methods.at(_k).get());
    if (_nonconst_callable_ptr != nullptr) {
        return _nonconst_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    throw std::invalid_argument("_R or sizeof...(_Args)");
};
template <typename _Tp> auto random_caller<_Tp>::run(const size_t _n) -> bool {
    if (_dist.empty()) { return true; }
    using _Iter = typename decltype(_dist)::const_iterator;
    _vro.density<_Iter>(_dist.cbegin(), _dist.cend(), [](_Iter _i) -> double {
        return _i->first;
    });
    _commands.clear(); _arguments.clear();
    for (size_t _i = 0; _i != _n; ++_i) {
        const std::string& _k = _dist.at(_vro.rand()).second;
        bool _saved = false;
        try {
            if (_constructors.contains(_k)) {
                _constructors.at(_k)->operator()();
            }
            else if (_methods.contains(_k)) {
                _methods.at(_k)->operator()();
            }
            _M_save_commands_and_arguments(_k); _saved = true;
            for (const auto& _callback : _callbacks) {
                _callback->operator()();
            }
        }
        catch (const std::exception& _e) {
            if (!_saved) _M_save_commands_and_arguments(_k);
            _exception = _e.what();
            return false;
        }
    }
    return true;
};


template <typename _Tp> auto random_caller<_Tp>::_M_call_copy_constructor() -> void {
    static_assert(std::is_copy_constructible<obj_type>::value);
    obj_type* _nc = this->_M_allocate_container(*_object);
    this->_M_deallocate_container(_object);
    _object = _nc;
};
template <typename _Tp> auto random_caller<_Tp>::_M_call_move_constructor() -> void {
    static_assert(std::is_move_constructible<obj_type>::value);
    obj_type* _nc = this->_M_allocate_container(std::move(*_object));
    this->_M_deallocate_container(_object);
    _object = _nc;
};
template <typename _Tp> auto random_caller<_Tp>::_M_save_commands_and_arguments(const std::string& _k) -> void {
    if (_constructors.contains(_k)) {
        _commands.push_back(_k);
        _arguments.push_back(_constructors.at(_k)->arguments());
    }
    else if (_methods.contains(_k)) {
        _commands.push_back(_k);
        _arguments.push_back(_methods.at(_k)->arguments());
    }
};

}

#endif // _ICY_CONTAINER_WRAPPER_HPP_