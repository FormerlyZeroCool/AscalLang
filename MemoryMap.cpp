/*
 * MemoryMap.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */

#include "MemoryMap.hpp"
#include "Object.hpp"
MemoryMap::MemoryMap(MemoryManager &data) : data(&data), map(data.node_pool) {}

void MemoryMap::clone(const MemoryMap &m)
{
    this->data = m.data;
    for(auto &[key,value] : m.map)
    {
        Object *obj = m.getMemMan().constructObj(*value);
        this->insert(*obj);
    }
}
MemoryMap& MemoryMap::operator=(const MemoryMap &m)
{
    this->clone(m);
    return *this;
}
MemoryMap::MemoryMap(const MemoryMap &m): map(m.getMemMan().node_pool)
{
    this->clone(m);
}
Object& MemoryMap::iterator::operator*() const
{
    return *((*intIt).second);
}
Object& MemoryMap::iterator::operator->() const
{
    return *((*intIt).second);
}
bool MemoryMap::iterator::operator==(Object &o) const
{
	return **this == o;
}
size_t MemoryMap::size()
{
	return this->map.getElementCount();
}
Object& MemoryMap::insert(string_view id)
{
    char c = 0;
    string_view exp(&c, 0);
    return this->insert(id, exp);
}
Object& MemoryMap::insert(string_view id, string_view exp)
{
    Object* obj = nullptr;
    const auto node = this->map.find(id);
    if(node)
    {
        obj = node->data.second;
        obj->copyToId(id);
        obj->copyToInstructions(exp);
    }
    else
    {
        obj = this->getMemMan().constructObj(id, exp);
        this->map.insert(obj->getId(), obj);
    }
    return *obj;
}
Object& MemoryMap::insert(Object &s)
{
    Object* obj = nullptr;
    const auto node = this->map.find(s.id);
	if(node)
	{
        obj = node->data.second;
        obj->clone(s);
	}
    else
    {
        obj = this->getMemMan().constructObj(s);
        this->map.insert(obj->getId(), obj);
    }
	return *obj;
}
//Dangerous, user needs to manage the string views memory
Object& MemoryMap::insert(string_view sv, Object &s)
{
    Object* obj = nullptr;
    string_view id = s.getId();
    const auto node = this->map.find(id);
    if(node)
    {
        obj = node->data.second;
        *obj = s;
    }
    else
    {
        obj = this->getMemMan().constructObj(s);
        this->map.insert(sv, obj);
    }
	return *obj;
}

Object& MemoryMap::operator[](string_view s)
{	
    const auto node = this->map.find(s);
    if(node)
		return *node->data.second;
	else
	{
		Object &obj = insert(s);
		return obj;
	}
}

node<string_view, Object*>* MemoryMap::search(string_view s)
{
    return this->map.find(s);
}
Object& MemoryMap::find(string_view s)
{
	return *this->map.find(s)->data.second;
}
size_t MemoryMap::count(string_view s) 
{
	return map.count(s);
}
void MemoryMap::erase(string_view s)
{
    const auto node = this->map.find(s);
	if(node)
	{
		this->getMemMan().obj_free(node->data.second);
		this->map.erase(s);
	}
}
void freeObjNode(node<string_view, Object*> *node, MemoryManager &m)
{
	m.obj_free(node->data.second);
}
void MemoryMap::clear()
{
    if(this->map.root)
	    this->map.freeRecursive(this->map.root, freeObjNode, this->getMemMan());
}
MemoryMap::~MemoryMap()
{
	clear();
}
MemoryMap::iterator MemoryMap::begin() { return MemoryMap::iterator(map.begin()); }
MemoryMap::iterator MemoryMap::end() { return MemoryMap::iterator(map.end()); }
