/*
 * MemoryMap.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */

#include "MemoryMap.hpp"
#include "Object.hpp"

MemoryMap::MemoryMap(MemoryManager &data) : data(&data) {}
Object& MemoryMap::iterator::operator*() const
{
    return *intIt->second;
}
Object& MemoryMap::iterator::operator->() const
{
    return *intIt->second;
}
bool MemoryMap::iterator::operator==(Object &o) const
{
	return **this == o;
}
size_t MemoryMap::size()
{
	return this->map.size();
}

Object& MemoryMap::insert(Object &s)
{
    Object* obj = nullptr;
	if(this->map.count(s.id))
	{
        obj = this->map[s.id];
        *obj = s;
	}
    else
    {
        obj = this->getMemMan().constructObj(s);
        this->map[obj->getId()] = obj;
    }
	return *obj;
}
//Dangerous, user needs to manage the string views memory
Object& MemoryMap::insert(string_view sv, Object &s)
{
    Object* obj = nullptr;
    string_view id = s.getId();
    if(this->map.count(id))
    {
        obj = this->map[sv];
        *obj = s;
    }
    else
    {
        obj = this->getMemMan().constructObj(s);
        this->map[sv] = obj;
    }
	return *obj;
}

Object& MemoryMap::operator[](string_view s)
{	if(map.count(s))
		return *map[s];
	else
	{
		Object o(*data ,s.str(),"","");
		Object &obj = insert(o);
		return obj;
	}
}
Object& MemoryMap::find(string_view s)
{
	return *map.find(s)->second;
}
size_t MemoryMap::count(string_view s)
{
	return map.count(s);
}
void MemoryMap::erase(string_view s)
{
	if(this->map.count(s))
	{
		this->getMemMan().obj_free(this->map[s]);
		this->map.erase(s);
	}
}
/*
void MemoryMap::erase(uint64_t addr)
{
    this->data->dealloc(addr);
}
 */
void MemoryMap::clear()
{
	std::set<Object* > cleared;
	for(auto &[key,value] : this->map)
	{
		if(!cleared.count(value))
		{
            this->getMemMan().obj_free(value);
			cleared.insert(value);
		}
	}
}
MemoryMap::~MemoryMap()
{
	clear();
}
MemoryMap::iterator MemoryMap::begin() { return MemoryMap::iterator(map.begin()); }
MemoryMap::iterator MemoryMap::end() { return MemoryMap::iterator(map.end()); }
MemoryManager& MemoryMap::getMemMan()
{
	return *data;
}

