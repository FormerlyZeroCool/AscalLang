/*
 * MemoryManager.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */
#include "MemoryManager.hpp"
#include "Object.hpp"

MemoryManager::MemoryManager(): small_obj_pool(Object::SMALL_EXP), medium_obj_pool(Object::MEDIUM_EXP), large_obj_pool(Object::LARGE_EXP), very_large_obj_pool(Object::VERYLARGE_EXP),
    small_obj_id_pool(Object::SMALL_ID), large_obj_id_pool(Object::LARGE_ID) {}
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
