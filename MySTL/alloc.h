#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

/*
如果分配的区块够大，超过128bytes时，则采用一级配置器进行空间分配。
否则，采用二级配置器进行分配。
二级配置器采用内存池（free-lists)进行空间区块的管理。
*/

/* free-lists
分别维护了16个free-list, 各自管理大小分别为 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128bytes 的小额区块。
在进行空间配置时，配置器会自动将任何小额区块的需求调升至8的倍数。
*/

//c++标准规范下的c头文件（无扩展名）
#include <new>
#include <cstdio>

namespace mystl    //命名空间: 建立一些互相分隔的作用域，把一些全局实体分隔开来。
{
    //free-lists 
    /*为了避免额外的开销（额外的指针指向下一个节点），在存储free-lists时选用union进行存储。*/

    union freeList
    {
        union freeList* next;  //指向下一个空闲区块
        char data[1];  //存储当前空间区块的首地址
    };


    //lists size 的限制参数
    //疑问：为什么不用int， 而是使用 enum？
    enum {_ALIGN = 8};   //区块的上调边界
    enum {_MAX_BYTES = 128};    //区块大小的最大值
    enum {_FREELISTS_NUM = _MAX_BYTES / _ALIGN};


    class _alloc_mystl
    {
        private:
        //ROUND UP 上调区块需求，满足8的倍数
        static size_t ROUND_UP(size_t bytes)
        {
            
        }
    }


}



#endif