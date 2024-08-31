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

### 抽象类定义

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

### 形参包展开

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

### 参数 tuple

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

### tuple 转参数包

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

### 无参成员函数

`callable_impl` 类型至少需要一个形参，我们只能对 `callable` 类型进行偏特化。

~~~cpp
template <typename _Container, typename _R> struct callable :
public virtual_callable {
    using container_type = _Container;
    using method_type = _R(container_type::*)(_Args...);
    void operator()() override {
        (_container->*_call)();
    }
private:
    container_type* const _container;
    const method_type _call;
};
~~~

至此，我们解决了普通类型的入参成员函数问题。
但对于入参类型是引用、常量的成员函数，仍然存在包括 `std::tuple` 初始化及赋值的**问题**。

### 类型萃取

首先，`random_object` 只能返回非引用类型的数据，`std::tuple` 也不能存储引用类型数据。
因此我们在 `callable_impl` 中，需要对 `_This` 类型进行类型退化，这里可以顺便把 `const` 修饰符给去掉。

但我们不能对类型参数包进行类型萃取的操作，如何设置当前继承节点中 `_tuple` 成员的类型成为了新的难点。

在 `std::tuple_cat` 的实现中有一个叫 `std::__tuple_cat_result` 的函数可以获取 `tuple` 拼接的类型。我们可以给出下面的代码：

~~~cpp
template <size_t _Index, typename _This, typename... _Rest> struct callable_impl : public callable_impl<_Index + 1, _Rest...> {
    using base = callable_impl<_Index + 1, _Rest...>;
    ~callable_impl() override = default;
    void operator()() override {
        base::operator()();
        _tuple = std::tuple_cat(std::make_tuple(_ro.rand()), base::_tuple);
    }
    using value_type = std::decay<typename std::remove_reference<_This>::type>::type;
    typename std::__tuple_cat_result<std::tuple<value_type>, decltype(base::_tuple)>::__type _tuple;
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
~~~

其中的 `value_type` 就是萃取后的类型。当然我们也容易自己实现一个 `std::__tuple_cat_result`，如下面代码所示：

~~~cpp
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

~~~

### const 成员函数

目前我们 `enroll` 函数的成员函数指针入参都是没有 `const` 修饰的，对于有 `const` 修饰的成员函数，存在无法匹配的问题。（这里是指不会改变对象数据的成员函数）

因此对 `enroll` 写出针对两种入参的版本，并增加 `_callable` 形参，使之能针对性的生成成员函数指针类型。

~~~cpp
template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable : public callable_impl<0, _Args...> {
    using container_type = _Container;
    using method_type = std::conditional<_Const, _R(container_type::*)(_Args...)const, _R(container_type::*)(_Args...)>::type;
    callable(container_type* _c, method_type _p) : _container(_c), _call(_p) {}
private:
    container_type* const _container;
    const method_type _call;
};
template <typename _Container, bool _Const, typename _R> struct callable<_Container, _Const, _R> : public virtual_callable {
    using container_type = _Container;
    using method_type = std::conditional<_Const, _R(container_type::*)()const, _R(container_type::*)()>::type;
    callable(container_type* _c, method_type _p) : _container(_c), _call(_p) {}
private:
    container_type* const _container;
    const method_type _call;
};
~~~