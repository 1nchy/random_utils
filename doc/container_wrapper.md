# Container Wrapper

## Introduction

定义 [container_wrapper](../include/container_wrapper.hpp) 类的初衷，是将待测试的容器类进行包装，通过注册字符串-成员函数键值对，完成对目标成员函数的自动调用，其入参则由 `random_object` 随机生成。

## Usage

~~~cpp
container::wrapper<std::set<char>> _wrapper;
_wrapper.enroll("clear", &std::set::clear);
_wrapper.enroll("size", &std::set::size);
_wrapper.enroll("insert", &std::set::insert);
_wrapper.enroll("erase", &std::set::erase);
_wrapper.enroll("count", &std::set::count);
_wrapper.random_call("insert"); // insert a random char
_wrapper.random_call("clear");
const auto _l = _wrapper.call<size_t>("size"); // must match
const auto _la = _wrapper.call<size_t>("count", 'a'); // must match
~~~

## Details

- 抽象类定义

要存储字符串-成员函数键值对，显然应该使用哈希表，而为了能够以统一的类型存储，势必要定义提供了可调用接口的纯虚类 `virtual_callable`。

~~~cpp
struct virtual_callable {
    virtual void operator()() = 0;
    virtual ~virtual_callable() = default;
};
~~~

用来展开参数包的 `callable` 类只需要继承并实现 `void operator()()` 接口即可。

~~~cpp
template <typename _Container, typename _R, typename... _Args> struct callable :
public virtual_callable {
    using container_type = _Container;
    using method_type = _R(container_type::*)(_Args...);
    void operator()() override {}
private:
    container_type* const _container;
    const method_type _call;
};
~~~

在调用的时候，只需要把随机生成的参数打包传递给 `_call` 就行了。

但首先，我们至少需要将 `_Args` 形参展开，才能生成随机的参数。

- 形参包展开

为了逐个展开 `_Args` 形参包，我们仿照 `std::tuple` 的设计，让 `callable` 在迭代继承的过程中，将形参包展开。

~~~cpp
template <size_t _Index, typename _This, typename... _Rest> struct callable_impl :
public callable_impl<_Index + 1, _Rest...> {
private:
    random_object<_This> _ro;
};
template <size_t _Index, typename _This> struct callable_impl :
public virtual_callable {
private:
    random_object<_This> _ro;
};
template <typename _Container, typename _R, typename... _Args> struct callable : public callable_impl<0, _Args...> {
    using base = callable_impl<0, _Args...>;
    using container_type = _Container;
    using method_type = _R(container_type::*)(_Args...);
    void operator()() override {
        base::operator()();
        // std::cout << base::_tuple << std::endl;
    }
private:
    container_type* const _container;
    const method_type _call;
};
~~~

在 `callable_impl` 的迭代继承中，通过 `_This` 形参，完成了对形参包的逐层展开，并构造出对应类型的 `random_object` 对象。
并且保持了 `callable` 对象对 `virtual_callable` 的继承。

但现在仍然存在一个**问题**，就是如何收集各级父类中 `random_object` 成员方法返回值。

- 参数 tuple

在迭代继承的过程中，我们可以用 `std::tuple` 拼接出生成的随机参数。

~~~cpp
template <size_t _Index, typename _This, typename... _Rest> struct callable_impl :
public callable_impl<_Index + 1, _Rest...> {
    using base = callable_impl<_Index + 1, _Rest...>;
    void operator()() override {
        base::operator()();
        _tuple = std::tuple_cat(std::make_tuple(_ro.rand()), base::_tuple);
    }
protected:
    std::tuple<_This, _Rest...> _tuple;
private:
    random_object<_This> _ro;
};
template <size_t _Index, typename _This> struct callable_impl :
public virtual_callable {
    void operator()() override {
        _tuple = std::make_tuple(_ro.rand());
    }
protected:
    std::tuple<_This> _tuple;
private:
    random_object<_This> _ro;
};
~~~

在 `void operator()()` 方法的实现中，我们从父类逐级向下拼接 `tuple` 数据，并最终传递给 `callable` 对象。

到现在，我们获得了 `tuple` 类型的参数元组，但仍然存在如何把元组数据传递给 成员函数 的**问题**。

- tuple 转参数包

我们没法显式的使用 `std::get<N>` 方法将元组数据一个一个的传递给 成员函数。
只能把参数元组转换成参数包了。

C++14 中提出了 `index_sequence` 类型，可以生成 `size_t` 类型的参数包，
结合 `std::get<N>` 方法，把元组转换成参数包。

~~~cpp
template <typename _Container, typename _R, typename... _Args> struct callable :
public callable_impl<0, _Args...> {
    using base = callable_impl<0, _Args...>;
    using container_type = _Container;
    using method_type = _R(container_type::*)(_Args...);
    void operator()() override {
        base::operator()();
        invoke_tuple(base::_tuple);
    }
private:
    template <typename _Tuple> void invoke_tuple(const _Tuple& _tuple) {
        _M_invoke_tuple(_tuple, std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <typename _Tuple, size_t... _N> void
    _M_invoke_tuple(const _Tuple& _tuple, std::index_sequence<_N...>) {
        (_container->*_call)(std::move(std::get<_N>(_tuple))...);
    }
private:
    container_type* const _container;
    const method_type _call;
};
~~~

至此，我们解决了需要传参的成员函数的包装问题。但对于没有入参的成员函数，在继承 `callable_impl` 类时会**报错**。

- 无参成员函数

我们只能对 `callable` 类型进行偏特化