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
#include <memory>
class Pool {
    std::vector<void*> pool;
    void* poolPtr;
    std::vector<void*> freeList;
    int_fast32_t chunkSize;
    int_fast64_t poolSize;
    int_fast64_t index = 0;
public:
    Pool(int_fast32_t chunkSize): chunkSize(chunkSize) { 
        pool.reserve(16);
        freeList.reserve(4096);
        poolSize = chunkSize<<12; 
        poolPtr = std::malloc(poolSize); 
        pool.push_back(poolPtr); 
    }
    Pool(int_fast32_t chunkSize, int_fast64_t poolSize): chunkSize(chunkSize), poolSize(poolSize) { poolPtr = std::malloc(poolSize); pool.push_back(poolPtr); }
    ~Pool()
    {
        for(auto block : this->pool)
            free(block);
    }
    void* malloc()
    {
        void* ptr = nullptr;
        if(freeList.empty())
        {
            if(index*chunkSize < poolSize)
                ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(poolPtr)+chunkSize*index++);
            else
            {
                poolPtr = std::malloc(poolSize);
                pool.push_back(poolPtr);
                index = 1;
                ptr = poolPtr;
            }
        }
        else
        {
            ptr = freeList.back();
            freeList.pop_back();
        }
        return ptr;
    }
    void free(void* ptr)
    {
        freeList.push_back(ptr);
    }
};
//in dev
template <typename t>
class Pool_t {
    struct Chunk {
        char data[sizeof(t)];
        Chunk *next = nullptr;
        Chunk(Chunk *next): next(next) {}
        Chunk() {}
    };
    std::deque<Chunk* > blocks;
    Chunk *last = nullptr;
    int_fast64_t poolSize;
    int_fast64_t index = 0;
public:
    void init()
    {   
        auto ptr = new Chunk[this->poolSize];
        blocks.push_back(ptr);
        this->initBlock(ptr, poolSize);
        last = ptr;
    }
    Pool_t() {
        this->poolSize = 1024;
        this->init();
    }
    Pool_t(int64_t blocks) {
        this->poolSize = blocks;
        this->init();
    }
    ~Pool_t()
    {
        for(auto block : this->blocks)
            delete[] block;
    }
    void initBlock(Chunk *block, long len)
    {
        Chunk *last = block;
        for(int i = 1; i < len; i++)
        {
            last->next = block + i;
            last = last->next;
        }
    }
    void* malloc()
    {
        Chunk *ptr = this->last->next;
        if(!ptr)
        {
            ptr = new Chunk[this->poolSize];
            this->last->next = ptr;
            this->blocks.push_back(ptr);
            this->initBlock(ptr, this->poolSize);
            this->poolSize <<= 1;
        }
        this->last = ptr;
        return reinterpret_cast<void*>(&ptr->data);
    }
    void free(void* ptr)
    {
        Chunk *p = reinterpret_cast<Chunk *>(ptr);
        p->next = this->last->next;
        this->last->next = p;
    }
};
template <typename t>
class ObjectPool {
    Pool pool;
public:
    ObjectPool(): pool(sizeof(t)){}
    template <typename u>
    t* construct(u &first)
    {
        const void *location = this->pool.malloc();
        return new ((void*) location) t(first);
    }
    
    template <typename u, typename v>
    t* construct(u &first, v &second)
    {
        const void *location = this->pool.malloc();
        return new ((void*)location) t(first, second);
    }
    template <typename u, typename v, typename x>
    t* construct(u &first, v &second, x &third)
    {
        const void *location = this->pool.malloc();
        return new ((void*) location) t(first, second, third);
    }
    template <typename u, typename v, typename x, typename y>
    t* construct(u &first, v &second, x &third, y &fourth)
    {
        const void *location = this->pool.malloc();
        return new ((void*)location) t(first, second, third, fourth);
    }
    void destroy(t* record)
    {
        record->~t();
        pool.free(record);
    }
};

#endif /* Pool_h */
