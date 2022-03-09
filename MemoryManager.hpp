/*
 * MemoryManager.hpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */

#ifndef MEMORYMANAGER_HPP_
#define MEMORYMANAGER_HPP_
#include <vector>
#include <set>
#include <map>
#include "string_view.hpp"
#include "stack.hpp"
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include "Pool.hpp"
class Object;
class MemoryManager {
private:
    Pool small_obj_pool;
    Pool medium_obj_pool;
    Pool large_obj_pool;
    Pool very_large_obj_pool;
    Pool small_obj_id_pool;
    Pool large_obj_id_pool;
    boost::object_pool<Object> object_pool;
public:
	MemoryManager();
	~MemoryManager();
    inline void* small_alloc();
    inline void* medium_alloc();
    inline void* large_alloc();
    inline void* vlarge_alloc();
    inline void* small_id_alloc();
    inline void* large_id_alloc();
    Object* constructObj(Object &obj);
    Object* constructObj(string_view id, string_view exp);
    inline void small_free(void *ptr);
    inline void medium_free(void *ptr);
    inline void large_free(void *ptr);
    inline void vlarge_free(void *ptr);
    inline void small_id_free(void *ptr);
    inline void large_id_free(void *ptr);
    void obj_free(Object *ptr);
};
void* MemoryManager::small_alloc()
{
    return this->small_obj_pool.malloc();
}
void* MemoryManager::medium_alloc()
{
    return this->medium_obj_pool.malloc();
}
void* MemoryManager::large_alloc()
{
    return this->large_obj_pool.malloc();
}
void* MemoryManager::vlarge_alloc()
{
    return this->very_large_obj_pool.malloc();
}
void* MemoryManager::small_id_alloc()
{
    return this->small_obj_id_pool.malloc();
}
void* MemoryManager::large_id_alloc()
{
    return this->large_obj_id_pool.malloc();
}
void MemoryManager::small_free(void *ptr)
{
    this->small_obj_pool.free(ptr);
}
void MemoryManager::medium_free(void *ptr)
{
    this->medium_obj_pool.free(ptr);
}
void MemoryManager::large_free(void *ptr)
{
    this->large_obj_pool.free(ptr);
}
void MemoryManager::vlarge_free(void *ptr)
{
    this->very_large_obj_pool.free(ptr);
}
void MemoryManager::small_id_free(void *ptr)
{
    this->small_obj_id_pool.free(ptr);
}
void MemoryManager::large_id_free(void *ptr)
{
    this->large_obj_id_pool.free(ptr);
}
#endif /* MEMORYMANAGER_HPP_ */
