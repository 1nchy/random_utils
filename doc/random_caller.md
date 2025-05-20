# Random Caller

## Introduction

模板类 [random_caller](../include/random_caller.hpp) 将指定类型进行包装，通过注册字符串-成员函数键值对，完成对目标成员函数的自动调用，其入参则由 `random_object` 随机生成。

`random_caller` 类工作流程如下：
1. 注册成员函数（包括构造函数）以及每轮操作结束时都会执行一次的回调函数
2. 调用 `run` 方法，同时指定操作轮次以及是否保存成员方法入参
3. 随机抽选一个成员函数，随机生成其入参并调用
4. 依次执行回调函数，并回到第 3 步
5. 倘若上面两个步骤抛出继承自 `std::exception` 的异常，保存错误信息并退出

## Implement

这里仅讨论对类成员函数的封装，因为对函数的封装是更简单的。

### 抽象类定义

要存储字符串-成员函数键值对，显然应该使用哈希表，而为了能够以统一的类型存储，势必要定义提供了可调用接口的纯虚类 `virtual_callable`。

~~~cpp
struct virtual_callable {
    virtual ~virtual_callable() = default;
    virtual auto operator()() -> void = 0;
    virtual auto arguments() const -> std::string = 0;
};
~~~

`operator()` 方法用于调用成员方。
`arguments` 方法用于以字符串形式返回函数入参，方便定位与复现问题。
目前 `arguments` 不是我们讨论的重点，后面忽略该方法。

用来展开参数包的 `callable_method` 类只需要继承并实现 `void operator()()` 接口即可。

下面是一个容易想到的 `callable_method` 类的定义，因为篇幅问题，我们忽略模板参数 `_Const` 的作用。

~~~cpp
template <typename _Container, bool _Const, typename _R, typename... _Args>
struct callable_method : public virtual_callable {
    using obj_type = _Container;
    using obj_pointer = obj_type*;
    using method_type = _R(container_type::*)(_Args...); // ignore `_Const`
    using tuple_type = std::tuple<_Args...>;
    void operator()() override {}
private:
    obj_pointer& _object;
    const method_type _call;
    tuple_type _tuple;
    random_object<tuple_type> _rot;
};
~~~

当调用 `operator()` 方法时，由 `_rot` 生成一个 `tuple` 并进行调用即可。

但这里存在两个问题：

1. 类型参数包 `_Args` 里面可能存在引用类型，这是不能作为形参传递给 `random_object` 类的。
2. `tuple` 如何转换为参数包，供成员方法调用。

### 引用类型萃取

我们将仿照 `random_object` 类对 `tuple` 的处理，通过层级继承的方式实现引用类型的萃取。

~~~cpp
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
~~~

### tuple 转参数包

我们没法显式的使用 `std::get<N>` 方法将元组数据一个一个的传递给成员函数，只能把参数元组转换成参数包。

C++14 中提出了 `index_sequence` 类型，可以生成 `size_t` 类型的参数包，结合 `std::get<N>` 方法，把元组转换成参数包。

~~~cpp
template <typename _Container, bool _Const, typename _R, typename... _Args> struct callable_method : public virtual_callable {
    using obj_type = _Container;
    using obj_pointer = obj_type*;
    using method_type = _R(obj_type::*)(_Args...)const; // ignore `_Const`
    using tuple_type = typename remove_references<_Args...>::tuple_type;
    callable_method(obj_pointer& _c, method_type _p) : _object(_c), _call(_p) {}
    virtual ~callable_method() override = default;
    auto operator()() -> void override {
        _tuple = _rot.rand();
        invoke_tuple();
    }
private:
    auto invoke_tuple() -> void {
        _M_invoke_tuple(std::make_index_sequence<sizeof...(_Args)>{});
    }
    template <size_t... _N> auto _M_invoke_tuple(std::index_sequence<_N...>) -> void {
        (_object->*_call)(std::forward<_Args>(std::get<_N>(_tuple))...);
    }
private:
    obj_pointer& _object;
    const method_type _call;
    tuple_type _tuple;
    random_object<tuple_type> _rot;
};
~~~
