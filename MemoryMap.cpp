/*
 * MemoryMap.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */

#include "MemoryMap.hpp"
#include "Object.hpp"

MemoryMap::MemoryMap(MemoryManager<Object> &data) : data(&data) {}

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
	if(this->map.count(s.id))
	{
		size_t address = this->map[s.id];
		this->data->dealloc(address);
	}

	size_t addr = -1;
	Object &obj = this->data->alloc(s, addr);
	map[obj.id] = addr;

	return obj;
}
Object& MemoryMap::insert(string_view sv, Object &s)
{
	if(this->map.count(s.id))
	{
		size_t address = this->map[s.id];
		this->data->dealloc(address);

	}
	size_t addr = -1;
	Object &obj = this->data->alloc(s, addr);
	map.insert(std::make_pair(sv, addr));
	return obj;
}

Object& MemoryMap::operator[](string_view s)
{	if(map.count(s))
		return (*data)[map[s]];
	else
	{
		Object o(*data ,s.str(),"","");
		Object &obj = insert(o);
		return obj;
	}
}
Object& MemoryMap::find(string_view s)
{
	return (*data)[map.find(s)->second];
}
size_t MemoryMap::count(string_view s)
{
	return map.count(s);
}
void MemoryMap::erase(string_view s)
{
	if(this->map.count(s))
	{
		this->data->dealloc(this->map[s]);
		this->map.erase(s);
	}
}
void MemoryMap::clear()
{
	std::set<size_t> cleared;
	for(auto &[key,value] : this->map)
	{
		if(!cleared.count(value))
		{
			this->data->dealloc(value);
			cleared.insert(value);
		}
	}
}
size_t MemoryMap::getIndex(string_view s)
{
	return this->map.find(s)->second;
}
Object& MemoryMap::getObject(size_t index)
{
	return (*this->data)[index];
}
MemoryMap::~MemoryMap()
{
	clear();
}
MemoryMap::iterator MemoryMap::begin() { return MemoryMap::iterator(*data, map.begin()); }
MemoryMap::iterator MemoryMap::end() { return MemoryMap::iterator(*data, map.end()); }
MemoryManager<Object>& MemoryMap::getMemMan()
{
	return *data;
}

