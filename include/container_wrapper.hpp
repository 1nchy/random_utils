#ifndef _ICY_CONTAINER_WRAPPER_HPP_
#define _ICY_CONTAINER_WRAPPER_HPP_

#include <cstddef>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>
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
}
template <typename... _Ts> std::ostream& operator<<(std::ostream& _os, const std::tuple<_Ts...>& _tuple) {
    _os << '[';
    tuple_print<sizeof...(_Ts), decltype(_tuple)>::print(_os, _tuple);
    _os << ']';
    return _os;
}

struct virtual_callable;
template <typename _Container, typename _R, typename... _Args> struct callable;
template <size_t _Index, typename _This, typename... _Rest> struct callable_impl;
struct virtual_callable {
    virtual void operator()() = 0;
    virtual ~virtual_callable() = default;
};


template <typename _Container, typename _R, typename... _Args> struct callable : public callable_impl<0, _Args...> {
    using base = callable_impl<0, _Args...>;
    using container_type = _Container;
    using method_type = _R(container_type::*)(_Args...);
    callable(container_type* _c, method_type _t) : _container(_c), _call(_t) {}
    void operator()() override {
        base::operator()();
        // std::cout << base::_tuple << std::endl;
        invoke_tuple(base::_tuple);
    }
    _R invoke_with_result(_Args&&... _args) {
        return (_container->*_call)(std::forward<_Args>(_args)...);
    }
private:
    template <typename _Tuple> void invoke_tuple(const _Tuple& _tuple) {
        _M_invoke_tuple(_tuple, std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <typename _Tuple, size_t... _N> void _M_invoke_tuple(const _Tuple& _tuple, std::index_sequence<_N...>) {
        (_container->*_call)(std::move(std::get<_N>(_tuple))...);
    }
private:
    container_type* const _container;
    const method_type _call;
};
template <typename _Container, typename _R> struct callable<_Container, _R> : public virtual_callable {
    using container_type = _Container;
    using method_type = _R(container_type::*)();
    callable(container_type* _c, method_type _t) : _container(_c), _call(_t) {}
    void operator()() override {
        (_container->*_call)();
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
protected:
    std::tuple<_This, _Rest...> _tuple;
private:
    random_object<_This> _ro;
};
template <size_t _Index, typename _This> struct callable_impl<_Index, _This> : public virtual_callable {
    ~callable_impl() override = default;
    void operator()() override {
        _tuple = std::make_tuple(_ro.rand());
    }
protected:
    std::tuple<_This> _tuple;
private:
    random_object<_This> _ro;
};


template <typename _Tp> class wrapper {
    using container_type = _Tp;
    enum { _cycle_length = 1000000ul };
public:
    template <typename _R, typename... _Args> auto
    enroll(const std::string&, _R(_Tp::*)(_Args...), double _prob = 1.0) -> void;
    auto random_call(const std::string&) -> void;
    template <typename _R, typename... _Args> auto
    call(const std::string&, _Args&&... _args) -> _R;
    void run(const size_t _length = _cycle_length);
private:
    container_type _container;
    std::unordered_map<std::string, std::shared_ptr<virtual_callable>> _enrollment;
    std::vector<std::pair<const double, const std::string>> _dist; // the probability distribution of methods
    random_object<void> _vro;
};


template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
enroll(const std::string& _k, _R(_Tp::*_f)(_Args...), double _prob) -> void {
    auto _ptr = std::make_shared<callable<container_type, _R, _Args...>>(&_container, _f);
    _enrollment[_k] = _ptr;
    _dist.emplace_back(_prob, _k);
};
template <typename _Tp> auto wrapper<_Tp>::random_call(const std::string& _k) -> void {
    _enrollment.at(_k)->operator()();
};
template <typename _Tp> template <typename _R, typename... _Args> auto wrapper<_Tp>::
call(const std::string& _k, _Args&&... _args) -> _R {
    auto* const _callable_ptr = dynamic_cast<callable<container_type, _R, _Args...>*>(_enrollment.at(_k).get());
    if (_callable_ptr == nullptr) {
        throw std::invalid_argument("_R or sizeof...(_Args)");
    }
    return _callable_ptr->invoke_with_result(std::forward<_Args>(_args)...);
};
template <typename _Tp> auto wrapper<_Tp>::run(const size_t _length) -> void {
    _vro.update_density(_dist.cbegin(), _dist.cend(), [](decltype(_dist)::const_iterator _i) {
        return _i->first;
    });
    for (size_t _i = 0; _i != _length; ++_i) {
        const std::string& _k = _dist.at(_vro.rand()).second;
        random_call(_k);
    }
};

}

}

#endif // _ICY_CONTAINER_WRAPPER_HPP_