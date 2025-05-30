# Random Utils

## Background

我曾经写了个[哈希表](https://github.com/1nchy/cpp-stl/blob/master/hash_table.hpp)，
尽管经过了较多的手动传参测试，但后来仍然发现代码中存在着缺陷。
不止如此，在后来实现诸如红黑树、并查集等结构的时候，同样遇到过测试方面的问题。

最开始，我通过生成大量随机操作序列的方式，对代码（主要是容器类型的数据机构）进行测试。
以哈希表为例，我们进行了上百万次的随机的 CURD 操作，并定义了 `check` 方法用于检测内部结构的问题。
详见[代码](https://github.com/1nchy/cpp-stl/blob/master/adebug.hpp)。

这个方法虽然把哈希表代码的缺陷挖掘出来了，但明眼人都能看出其诸多缺点：

 - 对随机操作序列的抽象层次较低。
 - 只能对容器的几个固定方法进行测试，如 `clear`，`add` 等在不同容器中参数个数相同的函数。
 - 对容器成员方法的绑定方式不灵活，只能基于上面的特性，把容器成员方法绑定到固定类型的成员变量中。
 - 对新增方法的测试不友好，需要增加操作序列 id、增加容器测试类的成员变量等。
 - 对某些传参较特殊的方法无法测试，例如 `insert` 等。

限于当时的代码及工程能力，没有抽象成更好的测试工具。

而在今天，正如该项目所展示的那样，我把这个测试工具进行了更好的抽象与优化。
包括了随机对象生成，更好的函数及成员方法包装。

关于测试用例的效率，这个确实只能力大砖飞，相比起大模型生成测试用例，这个的优势在于小巧方便，顺便在实现的过程中多学习一下 c++ 的模板知识。

> 在大模型面前抬不起头的日子到此为止了！
> 目前大模型生成的测试用例效果，只能说非常幽默。
> 覆盖范围小，断言质量低，测试场景十分简陋。
> 待测试的类或函数集合越复杂，大模型生成测试用例的效果就越差。

## Introduction

### random_object

随机对象的生成是该工具的基石，通过模板特化与概念约束，实现了对于多数普通类型的对象的生成。
如 `double`，`std::string`，`std::pair` 等类型。
而对于 `std::tuple` 类型，我们使用了多层的继承结构，将 `tuple` 类型参数包展开。
多层继承结构也有利于我们实现随机对象生成的约束功能。

### random_caller

成员方法的随机调用器需要将类型作为形参传递给 `random_caller`。
调用 `enroll` 注册成员方法，在设置字符串键的同时，还可以设定调用概率以及参数约束。

### random_caller<void>

该类型是针对普通函数的随机调用器的特化。使用方法与 `random_caller` 大致相同。

## Details

`random_caller` 类还是比较复杂的，具体的设计与构造过程，见[文档](./doc/random_caller.md)。