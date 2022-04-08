#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_


//采用内存池实现


#include <new.h>

#include <stdio.h>


namespace mystl    //命名空间: 建立一些互相分隔的作用域，把一些全局实体分隔开来。
{
    //不同的内存范围
    enum
    {
        EAlign128 = 8,
    };
}



#endif