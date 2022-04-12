#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <new>

namespace mystl 
{
    template <typename T1, typename T2>
    inline void* construct(T1* p, T2& value)
    {
        new(p) T1(value);     //掉用 T1::T1(value)
    }


    template <typename T>
    inline void destroy(T* pointer)
    {
        pointer->~T();
    }
}

#endif