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
#include "stack.hpp"
template <typename t>
class MemoryManager {
private:
	std::vector<t> data;
	stack<size_t> freelist;
public:
	MemoryManager();
	MemoryManager(size_t size);
	t& operator[](size_t);
	t& alloc(t &record, size_t &addr);
	t& alloc(t &&record, size_t &addr);
	void dealloc(size_t address);
	~MemoryManager();
};

template <typename t>
MemoryManager<t>::MemoryManager() {
	this->data.reserve(8192);
	this->freelist.reserve(8192);
}
template <typename t>
MemoryManager<t>::MemoryManager(size_t size) {
	this->data.reserve(size);
	this->freelist.reserve(size);
}

template <typename t>
MemoryManager<t>::~MemoryManager() {
}

template <typename t>
t& MemoryManager<t>::alloc(t &&record, size_t &addr)
{
	return alloc(record, addr);
}
template <typename t>
t& MemoryManager<t>::alloc(t &record, size_t &addr)
{
	if(this->freelist.isEmpty())
	{
		this->data.push_back(record);
		addr = this->data.size()-1;
	}
	else
	{
		this->freelist.top(addr);
		this->freelist.pop();
		this->data[addr] = record;
	}
	return this->data[addr];
}

template <typename t>
t& MemoryManager<t>::operator[](size_t index)
{
	return this->data[index];
}
template <typename t>
void MemoryManager<t>::dealloc(size_t address)
{
	/*if(std::find(freelist.begin(), freelist.end(), address) != freelist.end())
	{
		std::cout<<"addr: "<<address<<"\nobj: "<<data[address].toString()<<"\n";
		//throw std::string("Error adding to freelist address twice");
	}else*/
	this->freelist.push(address);
}

#endif /* MEMORYMANAGER_HPP_ */
