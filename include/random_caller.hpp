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
template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable_method;
template <typename _R, typename... _Args> struct callable_function;
template <typename _Container> struct callable_constructor;
struct virtual_callable {
    virtual auto operator()() -> void = 0;
    virtual ~virtual_callable() = default;
    virtual auto arguments() const -> std::string = 0;
};


namespace {

/**
 * @brief generate random arguments for calling
 * @details
 * reference type could not work in `random_object`,
 * implement `remove_references` to trait type from `_Args...`
*/
template <typename _This, typename... _Rest> struct remove_references;

template <typename _This, typename... _Rest> struct remove_references : public remove_references<_Rest...> {
protected:
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
public:
    using tuple_type = tuple_cat_result<value_type, typename remove_references<_Rest...>::tuple_type>::type;
};
template <typename _This> struct remove_references<_This> {
protected:
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
public:
    using tuple_type = std::tuple<value_type>;
};

}

template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable_method : public virtual_callable {
    using obj_type = _Container;
    using container_pointer = obj_type*;
    using method_type = std::conditional<_Const, _R(obj_type::*)(_Args...)const, _R(obj_type::*)(_Args...)>::type;
    using tuple_type = typename remove_references<_Args...>::tuple_type;
    callable_method(container_pointer& _c, method_type _p) : _object(_c), _call(_p) {}
    template <typename... _Tts> callable_method(container_pointer& _c, method_type _p, _Tts&&... _tts) : _object(_c), _call(_p) {
        _rot.bound(std::forward<_Tts>(_tts)...);
    }
    virtual ~callable_method() override = default;
    auto operator()() -> void override {
        _tuple = _rot.rand();
        /**
         * @details std::apply(_call, std::tuple_cat(std::forward_as_tuple(*_object), base::_tuple));
         * std::apply can't handle reference parameters.
         */
        invoke_tuple();
    }
    auto arguments() const -> std::string override {
        std::ostringstream _ss;
        _ss << _tuple;
        return _ss.str();
    }
    auto invoke_with_result(_Args&&... _args) -> _R {
        return (_object->*_call)(std::forward<_Args>(_args)...);
    }
private:
    auto invoke_tuple() -> void {
        _M_invoke_tuple(std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <size_t... _N> auto _M_invoke_tuple(std::index_sequence<_N...>) -> void {
        (_object->*_call)(std::forward<_Args>(std::get<_N>(_tuple))...);
    }
private:
    container_pointer& _object;
    const method_type _call;
    tuple_type _tuple;
    random_object<tuple_type> _rot;
};
template <typename _Container, bool _Const, typename _R> struct callable_method<_Container, _Const, _R> : public virtual_callable {
    using obj_type = _Container;
    using container_pointer = obj_type*;
    using method_type = std::conditional<_Const, _R(obj_type::*)()const, _R(obj_type::*)()>::type;
    callable_method(container_pointer& _c, method_type _p) : _object(_c), _call(_p) {}
    virtual ~callable_method() override = default;
    auto operator()() -> void override {
        (_object->*_call)();
    }
    auto arguments() const -> std::string override {
        return "";
    }
    auto invoke_with_result() -> _R {
        return (_object->*_call)();
    }
private:
    container_pointer& _object;
    const method_type _call;
};

template <typename _R, typename... _Args> struct callable_function : public virtual_callable {
    using function_type = _R(*)(_Args...);
    using tuple_type = typename remove_references<_Args...>::tuple_type;
    callable_function(function_type _p) : _call(_p) {}
    template <typename... _Tts> callable_function(function_type _p, _Tts&&... _tts) : _call(_p) {
        _rot.bound(std::forward<_Tts>(_tts)...);
    }
    virtual ~callable_function() override = default;
    auto operator()() -> void override {
        _tuple = _rot.rand();
        invoke_tuple();
    }
    auto arguments() const -> std::string override {
        std::ostringstream _ss;
        _ss << _tuple;
        return _ss.str();
    }
    auto invoke_with_result(_Args&&... _args) -> _R {
        return (*_call)(std::forward<_Args>(_args)...);
    }
private:
    auto invoke_tuple() -> void {
        _M_invoke_tuple(std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <size_t... _N> auto _M_invoke_tuple(std::index_sequence<_N...>) -> void {
        (*_call)(std::forward<_Args>(std::get<_N>(_tuple))...);
    }
private:
    const function_type _call;
    tuple_type _tuple;
    random_object<tuple_type> _rot;
};
template <typename _R> struct callable_function<_R> : public virtual_callable {
    using function_type = _R(*)();
    callable_function(function_type _p) : _call(_p) {}
    virtual ~callable_function() override = default;
    auto operator()() -> void override {
        (*_call)();
    }
    auto arguments() const -> std::string override {
        return "";
    }
    auto invoke_with_result() -> _R {
        return (*_call)();
    }
private:
    const function_type _call;
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



template <typename _Tp> struct random_caller_alloc;
template <typename _Tp> struct random_caller_impl;
template <typename _Tp = void> class random_caller;

template <typename _Tp> struct random_caller_alloc : public std::allocator<_Tp> {
    using obj_type = _Tp;
    typedef std::allocator<_Tp> container_allocator_type;
    typedef std::allocator_traits<container_allocator_type> container_alloc_traits;

    virtual ~random_caller_alloc() = default;

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

template <typename _Tp> struct random_caller_impl : public random_caller_alloc<_Tp> {
    static constexpr size_t loop = 1000000ul;
    using obj_type = _Tp;
protected:
    random_caller_impl() = default;
public:
    random_caller_impl(const random_caller_impl&) = delete;
    random_caller_impl& operator=(const random_caller_impl&) = delete;
    virtual ~random_caller_impl() = default;
public:
    /**
     * @brief random operation on the container specific times
     * @param loop the number of operations
     */
    auto run(const size_t _loop = loop) -> bool;
    const auto& get_commands() const { return _commands; }
    const auto& get_arguments() const { return _arguments; }
    const auto& get_exception() const { return _exception; }
protected:
    auto _M_save_commands_and_arguments(const std::string& _k) -> void;
protected:
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _callables;
    std::vector<std::shared_ptr<virtual_callable>> _callbacks;
    std::vector<std::pair<const double, const std::string>> _distribution; // the weight distribution of methods
    random_object<void> _vro;
    std::vector<std::string> _commands;
    std::vector<std::string> _arguments;
    std::string _exception;
};

/**
 * @brief container and method caller
 * @tparam _Tp container type
 */
template <typename _Tp> class random_caller : public random_caller_impl<_Tp> {
    using obj_type = _Tp;
public:
    random_caller();
    virtual ~random_caller();
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
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight = 1.0) -> bool;
    template <typename _R, typename... _Args> auto
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight = 1.0) -> bool;
    template <typename _R, typename... _Args, typename... _Tts> auto
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight, _Tts&&... _tts) -> bool;
    template <typename _R, typename... _Args, typename... _Tts> auto
    enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight, _Tts&&... _tts) -> bool;
    auto enroll_copy_construtor(double _weight = 1.0) -> bool;
    auto enroll_move_construtor(double _weight = 1.0) -> bool;
    template <typename _R, typename... _Args> auto
    push_callback(_R(_Tp::*_p)(_Args...)) -> void;
    template <typename _R, typename... _Args> auto
    push_callback(_R(_Tp::*_p)(_Args...)const) -> void;
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
private:
    void _M_call_copy_constructor();
    void _M_call_move_constructor();
private:
    obj_type* _object;
};

template <> class random_caller<void> : public random_caller_impl<void> {
    using self = random_caller<void>;
    using obj_type = void;
public:
    random_caller() = default;
    virtual ~random_caller() = default;
public:
    template <typename _R, typename... _Args> auto
    enroll(const std::string& _k, _R(*_p)(_Args...), double _weight = 1.0) -> bool;
    template <typename _R, typename... _Args> auto
    push_callback(_R(*_p)(_Args...)) -> void;
    template <typename _R, typename... _Args> auto
    call(const std::string& _k, _Args&&... _args) -> _R;
};



template <typename _Tp> auto random_caller_impl<_Tp>::_M_save_commands_and_arguments(const std::string& _k) -> void {
    if (_callables.contains(_k)) {
        _commands.push_back(_k);
        _arguments.push_back(_callables.at(_k)->arguments());
    }
};
template <typename _Tp> auto random_caller_impl<_Tp>::run(const size_t _loop) -> bool {
    if (_distribution.empty()) { return true; }
    using _Iter = typename decltype(_distribution)::const_iterator;
    _vro.density<_Iter>(_distribution.cbegin(), _distribution.cend(), [](_Iter _i) -> double {
        return _i->first;
    });
    _commands.clear(); _arguments.clear();
    for (size_t _i = 0; _i != _loop; ++_i) {
        const std::string& _k = _distribution.at(_vro.rand()).second;
        bool _saved = false;
        try {
            if (_callables.contains(_k)) {
                _callables.at(_k)->operator()();
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



template <typename _Tp> random_caller<_Tp>::random_caller() : random_caller_impl<_Tp>() {
    _object = this->_M_allocate_container();
};
template <typename _Tp> random_caller<_Tp>::~random_caller() {
    this->_M_deallocate_container(_object);
};

template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight) -> bool {
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_method<obj_type, false, _R, _Args...>>(std::ref(_object), _p);
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight) -> bool {
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_method<obj_type, true, _R, _Args...>>(std::ref(_object), _p);
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> template <typename _R, typename... _Args, typename... _Tts> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...), double _weight, _Tts&&... _tts) -> bool {
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_method<obj_type, false, _R, _Args...>>(std::ref(_object), _p, std::forward<_Tts>(_tts)...);
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> template <typename _R, typename... _Args, typename... _Tts> auto random_caller<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_p)(_Args...)const, double _weight, _Tts&&... _tts) -> bool {
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_method<obj_type, true, _R, _Args...>>(std::ref(_object), _p, std::forward<_Tts>(_tts)...);
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> auto random_caller<_Tp>::enroll_copy_construtor(double _weight) -> bool {
    static_assert(std::is_copy_constructible<obj_type>::value);
    const std::string _k = "copy_constructor";
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_constructor<obj_type>>(std::ref(_object), [this]() {
        this->_M_call_copy_constructor();
    });
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> auto random_caller<_Tp>::enroll_move_construtor(double _weight) -> bool {
    static_assert(std::is_move_constructible<obj_type>::value);
    const std::string _k = "move_constructor";
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_constructor<obj_type>>(std::ref(_object), [this]() {
        this->_M_call_move_constructor();
    });
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
push_callback(_R(_Tp::*_p)(_Args...)) -> void {
    auto _ptr = std::make_shared<callable_method<obj_type, false, _R, _Args...>>(std::ref(_object), _p);
    this->_callbacks.emplace_back(_ptr);
}
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
push_callback(_R(_Tp::*_p)(_Args...)const) -> void {
    auto _ptr = std::make_shared<callable_method<obj_type, true, _R, _Args...>>(std::ref(_object), _p);
    this->_callbacks.emplace_back(_ptr);
}
template <typename _Tp> template <typename _R, typename... _Args> auto random_caller<_Tp>::
call(const std::string& _k, _Args&&... _args) -> _R {
    auto* _const_callable_ptr = dynamic_cast<callable_method<obj_type, true, _R, _Args...>*>(this->_callables.at(_k).get());
    if (_const_callable_ptr != nullptr) {
        return _const_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    auto* _nonconst_callable_ptr = dynamic_cast<callable_method<obj_type, false, _R, _Args...>*>(this->_callables.at(_k).get());
    if (_nonconst_callable_ptr != nullptr) {
        return _nonconst_callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    throw std::invalid_argument("_R or sizeof...(_Args)");
};



template <typename _R, typename... _Args> auto random_caller<void>::
enroll(const std::string& _k, _R(*_p)(_Args...), double _weight) -> bool {
    if (this->_callables.contains(_k)) { return false; }
    auto _ptr = std::make_shared<callable_function<_R, _Args...>>(_p);
    this->_callables[_k] = _ptr;
    this->_distribution.emplace_back(_weight, _k);
    return true;
};
template <typename _R, typename... _Args> auto random_caller<void>::
push_callback(_R(*_p)(_Args...)) -> void {
    auto _ptr = std::make_shared<callable_function<_R, _Args...>>(_p);
    this->_callbacks.emplace_back(_ptr);
};
template <typename _R, typename... _Args> auto random_caller<void>::
call(const std::string& _k, _Args&&... _args) -> _R {
    auto* _ptr = dynamic_cast<callable_function<_R, _Args...>*>(this->_callables.at(_k).get());
    if (_ptr != nullptr) {
        return _ptr->invoke_with_result(std::forward<_Args>(_args)...);
    }
    throw std::invalid_argument("_R or sizeof...(_Args)");
};



template <typename _Tp> auto random_caller<_Tp>::_M_call_copy_constructor() -> void {
    obj_type* _new_object = this->_M_allocate_container(*_object);
    this->_M_deallocate_container(_object);
    _object = _new_object;
};
template <typename _Tp> auto random_caller<_Tp>::_M_call_move_constructor() -> void {
    obj_type* _new_object = this->_M_allocate_container(std::move(*_object));
    this->_M_deallocate_container(_object);
    _object = _new_object;
};

}

#endif // _ICY_CONTAINER_WRAPPER_HPP_