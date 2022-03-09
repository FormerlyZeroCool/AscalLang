//
//  Pool.hpp
//  AscalLang
//
//  Created by Andrew Rubinstein on 4/25/21.
//  Copyright © 2021 Andrew Rubinstein. All rights reserved.
//

#ifndef Pool_h
#define Pool_h
#include "stack.hpp"
#include <cinttypes>
#include <cstdlib>
class Pool {
    stack<void*> pool;
    void* poolPtr;
    stack<void*> freeList;
    int_fast32_t chunkSize;
    int_fast64_t poolSize;
    int_fast64_t index = 0;
public:
    Pool(int_fast32_t chunkSize): chunkSize(chunkSize) { poolSize = (1<<20)<chunkSize*500?1<<20:1<<30; poolPtr = std::malloc(poolSize); pool.push(poolPtr); }
    Pool(int_fast32_t chunkSize, int_fast64_t poolSize): chunkSize(chunkSize), poolSize(poolSize) { poolPtr = std::malloc(poolSize); pool.push(poolPtr); }
    void* malloc()
    {
        void* ptr = nullptr;
        if(freeList.isEmpty())
        {
            if(index*chunkSize < poolSize)
                ptr = static_cast<void*>(static_cast<void**>(poolPtr)+chunkSize*index++);
            else
            {
                poolPtr = std::malloc(poolSize);
                pool.push(poolPtr);
                index = 1;
                ptr = poolPtr;
            }
        }
        else
        {
            freeList.top(ptr);
            freeList.pop();
        }
        return ptr;
    }
    void free(void* ptr)
    {
        freeList.push(ptr);
    }
};

template <typename t>
class ObjectPool {
    Pool pool;
public:
    
    template <typename u>
    void construct(u &first);
    void construct(t &data);
};

#endif /* Pool_h */
