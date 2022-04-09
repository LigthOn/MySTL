#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

//如果分配的区块够大，超过128bytes时，则采用一级配置器进行空间分配。
//STL的二级配置器
//采用内存池实现

//c++标准规范下的c头文件（无扩展名）
#include <new>
#include <cstdio>


namespace mystl    //命名空间: 建立一些互相分隔的作用域，把一些全局实体分隔开来。
{
    //不同的内存范围
    enum
    {
        EAlign128 = 8,
    };
}



#endif