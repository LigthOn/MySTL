# MySTL
Github地址:https://github.com/LigthOn/MySTL
                                                                                                                                                        
参考Github地址:https://github.com/Alinshans/MyTinySTL

参考wiki文档:https://github.com/Alinshans/MyTinySTL/wiki


STL六大组件
1、容器（Containers）：各种数据结构，如Vector,List,Deque,Set,Map,用来存放数据，STL容器是一种Class Template,就体积而言，这一部分很像冰山载海面的比率。

2、算法（Algorithms）：各种常用算法如Sort,Search,Copy,Erase,从实现的角度来看，STL算法是一种Function Templates。

3、迭代器（Iterators）：扮演容器与算法之间的胶合剂，是所谓的“泛型指针”，共有五种类型，以及其它衍生变化，从实现的角度来看，迭代器是一种将：Operators*,Operator->,Operator++,Operator--等相关操作予以重载的Class Template。所有STL容器都附带有自己专属的迭代器——是的，只有容器设计者才知道如何遍历自己的元素，原生指针（Native pointer）也是一种迭代器。

4、仿函数（Functors）： 行为类似函数，可作为算法的某种策略（Policy）,从实现的角度来看，仿函数是一种重载了Operator()的Class 或 Class Template。一般函数指针可视为狭义的仿函数。

5、配接器（适配器）（Adapters）：一种用来修饰容器（Containers）或仿函数（Functors）或迭代器（Iterators）接口的东西，例如：STL提供的Queue和Stack，虽然看似容器，其实只能算是一种容器配接器，因为 它们的底部完全借助Deque，所有操作有底层的Deque供应。改变Functor接口者，称为Function Adapter;改变Container接口者，称为Container Adapter;改变Iterator接口者，称为Iterator Adapter。配接器的实现技术很难一言蔽之，必须逐一分析。

6、分配器（Allocators）：负责空间配置与管理，从实现的角度来看，配置器是一个实现了动态空间配置、空间管理、空间释放的Class Template。

                                                                                                 ——《STL源码剖析》






代码实现部分：

1. 分配器 （Allocators)

包括 allocator 和 constructor，

allocator 负责内存空间的配置与回收，定义了一个MySTL:allocator类，并使用内存池实现。

constructor 负责对象的构造与析构。


<alloc.h>       //实现STL的二级空间配置.

在C++中，通常使用new和delete来实现空间的配置（注：这里的空间不仅仅是指内存空间，还包括磁盘或者其他存储介质），类似于C中的malloc和free.

mystl::alloc的实现原理：

1）设置一个空间大小阈值，通常设置为128bytes。

2）若申请的空间大小大于128bytes，则调用一级空间配置器。

3）若申请的空间大小小于128bytes，则调用二级空间配置器。



