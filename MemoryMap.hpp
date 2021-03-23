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
	MemoryManager *data;
    std::map<string_view, Object* , std::less<string_view> > map;
    uint32_t hashKey(string_view key);
public:
    MemoryMap(MemoryManager &data);
    MemoryMap(const MemoryMap &m) = default;
    MemoryMap(MemoryMap &m) = default;
    ~MemoryMap();
	void clear();
    void erase(string_view);
   // void erase(uint64_t);
    MemoryManager& getMemMan();
    Object& operator[](string_view);
    size_t count(string_view);
    Object& find(string_view);
    size_t size();
    //Prefferred methods to insert
    Object& insert(Object &obj);
    Object& insert(string_view id, string_view exp);
    Object& insert(string_view id);
    //Dangerous, user needs to manage the string views memory
    Object& insert(string_view s, Object &obj);
	class iterator {
	private:
		std::map<string_view, Object* >::iterator intIt;
	public:
		iterator(std::map<string_view, Object* >::iterator intIt): intIt(intIt) {}
        Object& operator*() const;
        Object& operator->() const;
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
			return this->intIt == o.intIt;
		}
		bool operator==(Object &o) const;
		bool operator!=(const MemoryMap::iterator &o) const
		{
			return this->intIt != o.intIt;
		}
	};
	MemoryMap::iterator begin();
	MemoryMap::iterator end();
};

#endif /* MEMORYMAP_HPP_ */
