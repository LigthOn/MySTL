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

        //当前内存池空闲空间的起始地址
        static char* start_free_pool;
        static char* end_free_pool;
        static size_t heap_size; //用于记录向系统heap申请内存空间大小总和


        //基本的 allocate 以及 deallocate 函数
        public:
        static void* allocate(size_t n);
        static void deallocate(void* p, size_t n);
        static void* reallocte(size_t old_n, void* p, size_t new_n);
    };

    //static 变量初始化
    freeList* volatile _alloc_mystl::free_lists[_FREELISTS_NUM] = 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    char* _alloc_mystl::start_free_pool = 0;
    char* _alloc_mystl::end_free_pool = 0;
    size_t _alloc_mystl::heap_size = 0;
    


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

    void* _alloc_mystl::reallocte(size_t old_n, void* p, size_t new_n)
    {
        deallocate(p, old_n);
        p = allocate(new_n);
        return p;
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
        freeList* result;    //返回分配的空闲区块
        freeList* curr_link, * next_link;

        if(nfreelist_links == 1)    //只能申请到一个符合条件的区块,即chunk
        {
            return chunk;
        }
        else   //获取的chunk可能是一大片区块，可后续分为返回多个区块，将其放入到 free-lists 中
        {   
            result = reinterpret_cast<freeList*>(chunk);  //返回给请求对象

            //将分配都的chunk 更新到对应的 free-list 中
            my_free_list = free_lists + FREELISTS_INDEX(n);  
            *my_free_list = next_link = reinterpret_cast<freeList*>(chunk + n);  
            
            //前 n bytes (0号区块）分配给result,后续（1 ~ nfreelist_links-1）的更新到free-list中
            for(int i = 1; ; ++i)
            {
                curr_link = next_link;   //此时 curr_link 已经指向了 my_free_list
                next_link = reinterpret_cast<freeList*>((char *)curr_link + n);  //(char *)curr_link 取地址

                if(i == nfreelist_links-1)  //最后一个区块
                {
                    curr_link->next = 0;
                    break;
                }
                else
                {
                    curr_link->next = next_link;
                }
            }
        }
        return result;
    }

    //因为要根据内存池具体的区块供应能力修改nfreelist_links，所有使用 引用的方式传递参数
    char* _alloc_mystl::chunk_alloc(size_t n, int& nfreelist_links)   //返回的是首地址
    {
        char* result;
        size_t required_bytes = n * nfreelist_links;
        size_t left_bytes = end_free_pool - start_free_pool;  //当前内存池中剩余的空闲空间大小

        if(left_bytes >= required_bytes)  //足够进行分配
        {
            result = start_free_pool;
            start_free_pool += required_bytes;
            return result;
        }
        else if(left_bytes >= n) //空间不够，但是能够分配一个
        {
            nfreelist_links = left_bytes / n;
            required_bytes = nfreelist_links * n;
            result = start_free_pool;
            start_free_pool += required_bytes;
            return result;
        }
        else  //一个满足需求的区块都无法分配
        {
            //先将当前剩余的空间 分配到 合适的 free-list中(肯定是只剩一个)
            if(left_bytes > 0)  //剩余价值利用
            {
                freeList* volatile *my_free_list = free_lists + FREELISTS_INDEX(left_bytes);
                reinterpret_cast<freeList*>(start_free_pool)->next = *my_free_list;
                *my_free_list = reinterpret_cast<freeList*>(start_free_pool);
            }

            //配置heap空间（向系统堆申请空间），用来补充内存池
            //bytes_to_get 需要向heap申请的空间的大小
            /*一般分配 required_bytes * 2
              同时为了应对越来越大的需求，根据请求的增多，扩大申请空间的大小(利用ROUND_UP(heap_size >> 4))
            */
            /*通过heap更新 start_free_pool 以及 end_free_pool，
              此时就是先更新了 内存池，
              然后再掉用 chunk_alloc 函数，在更新后的内存池中找到空间区用于 refill中更新 free-list
            */

            size_t bytes_to_get = required_bytes * 2 + ROUND_UP(heap_size >> 4);
            start_free_pool = (char *)std::malloc(bytes_to_get);

            if(start_free_pool == 0) //heap中也没有可用空间了
            {
                /*看看free-lists中还有没有剩余的其他的(size > n)的区块,
                  如果有的话，可以拿过来用一用*/
                
                for(int i = n; i < _MAX_BYTES; i += _ALIGN)
                {
                    freeList* volatile* my_free_list = free_lists + FREELISTS_INDEX(i);
                    freeList* p = *my_free_list;

                    if(p != 0) //找到了一个未使用的空间
                    {
                        *my_free_list = p->next;  //记得更新所占用的那个 free-list
                        start_free_pool = (char*)p;
                        end_free_pool = start_free_pool+i;

                        //递归自己，修正nfreelist_links,保证编入 free-list的没有多余的空间，而是正好是n
                        return chunk_alloc(n, nfreelist_links);  
                    }
                }
                //遍历free-lists之后还是没有找到空间的区块
                std::printf("OUT OF MEMORY!");
                throw std::bad_alloc();   //抛出异常
            }
            else
            {
                heap_size += bytes_to_get;
                end_free_pool = start_free_pool + bytes_to_get;
                return chunk_alloc(n, nfreelist_links);
            }
        }
    }
}
#endif