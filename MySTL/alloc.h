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
    enum {_FREELISTS_NUM = _MAX_BYTES / _ALIGN};  //区块size的个数


    class _alloc_mystl
    {
        private:
        //征对不同size的区块，定义16个freeLists
        //添加了 volatile 参数，表示该列表通常改变。
        static freeList * volatile free_lists[_FREELISTS_NUM];

        //ROUND UP 上调区块需求，满足8的倍数，使用静态可以减少实例化时额外的开销
        static size_t ROUND_UP(size_t bytes)
        {
            return (((bytes) + (_ALIGN - 1)) &  ~(_ALIGN - 1));   //虽然没有搞懂原理，但是验证了一下（位运算） 
        }

        //根据 区块的需求大小，获取其在 free_lists 中的index
        static size_t FREELISTS_INDEX(size_t bytes)
        {
            return (((bytes) + (_ALIGN - 1)) / (_ALIGN) - 1);
        }

        //重新填充 free_lists  !!!待更新
        static void* refill(size_t n);
        static char* chunk_alloc(size_t n, int& nfreelist_links);


        //基本的 allocate 以及 deallocate 函数
        public:
        static void* allocate(size_t n);
        static void deallocate(void* p, size_t n);
    };


    //分配需求为 n
    void* _alloc_mystl::allocate(size_t n)
    {
        freeList* volatile * my_free_list; //指向一个 freeList 指针对象的的指针
        freeList* volatile result;

        //如果大于 128bytes，则直接掉用 malloc
        if( n > _MAX_BYTES)
        {
            return std::malloc(n);
        }
        
        //找到最适合的 free-list
        my_free_list = free_lists  + FREELISTS_INDEX(n);
        //my_free_list = free_lists[ FREELISTS_INDEX(n) ];
        result = *my_free_list;
        if(result == 0)  //若没有空闲空间了，则掉用 refill 重新填充 free-list.
        {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        else //找到合适的区块用于分配
        {
            *my_free_list = result->next;  //调整free_lists.
            return result;
        }
    }


    void _alloc_mystl::deallocate(void *p, size_t n)
    {
        freeList* volatile *my_free_list;
        freeList* q = reinterpret_cast<freeList*>(p);

        my_free_list = free_lists + FREELISTS_INDEX(n);

        //n大于 128 bytes，则调用一级 free;
        if(n > _MAX_BYTES)
        {
            std::free(p);
        }
        else
        {
            my_free_list = free_lists + FREELISTS_INDEX(n);
            q->next = *my_free_list;
            *my_free_list = q;
        }
    }

    /*
    在调用allocate时，若没有空闲空间，则掉用refill来进行free-lists的更新，并返回一个大小为n的区块
    新的区块空间取自内存池，经由 chunk_alloc() 完成。
    缺省取得20个新区块
    当内存池空间不足时，获得的新区快的数量可能低于20.
    */
    void* _alloc_mystl::refill(size_t n)
    {
        int nfreelist_links = 20;  //该数值会根据当前内存池可供应区块的能力进行调整（会减小）
                                   //在后面通过 引用 的方式 更改值 (pass by reference)
        
        //nfreelist_links change by chunk_alloc(size_t n, int &nfreelist_links)]
        char* chunk = chunk_alloc(n, nfreelist_links);

        freeList* volatile *my_free_list;

        if(nfreelist_links == 1)    //只能申请到一个符合条件的区块,即chunk
        {
            return chunk;
        }
        else      //返回多个区块，将其放入到 free-lists 中
        {
            my_free_list = free_lists + FREELISTS_INDEX(n);
        }



        


    }

}
#endif