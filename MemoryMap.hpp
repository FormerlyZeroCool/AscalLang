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
    Map<string_view, Object* > map;
    uint32_t hashKey(string_view key);
public:
	friend Object;
    MemoryMap(MemoryManager &data);
	void clone(const MemoryMap &m);
    MemoryMap(const MemoryMap &m);
	MemoryMap& operator=(const MemoryMap &m);
    ~MemoryMap();
	void clear();
    void erase(string_view);
    inline MemoryManager& getMemMan() const
    {
        return *data;
    }

    Object& operator[](string_view);
    size_t count(string_view) const;
    Object& find(string_view);
    node<string_view, Object*>* search(string_view);
    size_t size();
    //Preferred methods to insert
    Object& insert(Object &obj);
    Object& insert(string_view id, string_view exp);
    Object& insert(string_view id);
    //Dangerous, user needs to manage the string view's memory
    Object& insert(string_view s, Object &obj);
	size_t count(string_view s);
	class iterator {
	private:
		Map<string_view, Object* >::iterator<string_view, Object* > intIt;
	public:
		iterator(Map<string_view, Object* >::iterator<string_view, Object* > intIt): intIt(intIt) {}
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
