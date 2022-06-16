/*
 * MemoryManager.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */
#include "MemoryManager.hpp"
#include "Object.hpp"
typedef std::allocator<std::map<int, std::string, std::less<int>>::value_type> MyAlloc;

std::map<int, std::string, std::less<int>, MyAlloc> m;

MemoryManager::MemoryManager(): small_obj_pool(SMALL_EXP), medium_obj_pool(MEDIUM_EXP), large_obj_pool(LARGE_EXP), very_large_obj_pool(VERYLARGE_EXP),
    small_obj_id_pool(SMALL_ID), large_obj_id_pool(LARGE_ID) {}
MemoryManager::~MemoryManager() {}

Object* MemoryManager::constructObj(Object &obj)
{
    return this->object_pool.construct(obj);
}
Object* MemoryManager::constructObj(string_view id, string_view exp)
{
    return this->object_pool.construct(*this, id, exp);
}
void MemoryManager::obj_free(Object *ptr)
{
    this->object_pool.destroy(ptr);
}
