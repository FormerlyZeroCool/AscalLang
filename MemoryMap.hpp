/*
 * MemoryMap.hpp
 *
 *  Created on: Feb 20, 2021
 *      Author: andrewrubinstein
 */

#ifndef MEMORYMAP_HPP_
#define MEMORYMAP_HPP_
#include <map>
#include "string_view.hpp"
#include "MemoryManager.hpp"
class Object;
class MemoryMap {
private:
	MemoryManager<Object> *data;
	std::map<string_view, size_t > map;
public:
	void clear();
	void erase(string_view);
	MemoryMap(const MemoryMap &m) = default;
	MemoryMap(MemoryMap &m) = default;
	class iterator {
	private:
		std::map<string_view, size_t >::iterator intIt;
		MemoryManager<Object> &man;
	public:
		iterator(MemoryManager<Object> &man, std::map<string_view, size_t >::iterator intIt): intIt(intIt), man(man) {}
		Object& operator*() const
		{
			return man[intIt->second];
		}
		Object& operator->() const
		{
			return man[intIt->second];
		}
		MemoryMap::iterator operator++()
		{
			++intIt;
			return *this;
		}
		MemoryMap::iterator operator--()
		{
			--intIt;
			return *this;
		}
		MemoryMap::iterator operator++(int)
		{
			MemoryMap::iterator intIt1(*this);
			++*this;
			return intIt1;
		}
		MemoryMap::iterator operator--(int)
		{
			MemoryMap::iterator intIt1(*this);
			--*this;
			return intIt1;
		}
		bool operator==(const MemoryMap::iterator &o) const
		{
			return &this->man == &o.man && this->intIt == o.intIt;
		}
		bool operator==(Object &o) const;
		bool operator!=(const MemoryMap::iterator &o) const
		{
			return &this->man != &o.man || this->intIt != o.intIt;
		}
	};
	MemoryMap(MemoryManager<Object> &data);
	MemoryMap::iterator begin();
	MemoryMap::iterator end();
	MemoryManager<Object>& getMemMan();
	Object& operator[](string_view);
	size_t count(string_view);
	Object& find(string_view);
	size_t size();
	Object& insert(Object &obj);
	Object& insert(string_view s, Object &obj);
	size_t getIndex(string_view s);
	Object& getObject(size_t index);
	~MemoryMap();
};

#endif /* MEMORYMAP_HPP_ */
