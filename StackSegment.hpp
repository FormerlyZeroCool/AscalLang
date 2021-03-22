/*
 * StackSegment.h
 *
 *  Created on: Mar 1, 2021
 *      Author: andrewrubinstein
 */

#ifndef STACKSEGMENT_HPP_
#define STACKSEGMENT_HPP_
#include "stack.hpp"
template <typename t>
class StackSegment {
private:
	size_t start, len;
	stack<t> *data;
public:
    //only used while returning value from function into previous function's operand stack
    inline void resetStart()
    {
        this->start = this->data->size();
    }
	inline void setData(stack<t> &dat)
	{
		data = &dat;
		start = dat.size();
	}
	void clear()
	{
        if(start+len == data->size() || !len){
        const size_t length = len;
		for(size_t i = 0; i < length; i++)
            this->pop();
        }
        else {
            std::cout<<(start+len)<<" "<<data->size()<<("Error clearing segment not at top of stack\n");
        }
	}
	~StackSegment()
	{
		clear();
	}
	StackSegment(stack<t> &dat);
	StackSegment() {start = 0; len = 0; data = nullptr;}
	inline t& operator[](size_t index)
	{
        if(index < len)
            return (*data)[index+start];
        else
            throw std::string("Invalid access "+std::to_string(index));
	}
	inline void push(const t &data);
	inline void push(const t &&data);
    inline void push_back(const t &data)
	{
		push(data);
	}
	inline void push_back(const t &&data)
	{
		push(data);
	}
	inline void pop();
    inline void pop_back() { pop(); }
	inline void top(t&data);
	inline void top(t*&data);
	inline bool isEmpty();
	inline size_t length();
	inline size_t size() { return length(); }

	template <typename u>
	class iterator {
	private:
		size_t index;
		stack<u> *data;
	public:
		iterator(size_t index, stack<u> *data): index(index),data(data) {}
		bool operator==(iterator<u> &o)
		{
			return this->data == o.data &&
					this->index == o.index;
		}
		bool operator!=(iterator<u> &o)
		{
			return this->index != o.index ||
					this->data != o.data;
		}
		u& operator*()
		{
			return (*data)[index];
		}
		u& operator->()
		{
			return (*data)[index];
		}
		iterator<u>& operator++()
		{
			index++;
			return *this;
		}
		iterator<u> operator++(int)
		{
			iterator<u> copy(*this);
			index++;
			return copy;
		}
		iterator<u>& operator+=(size_t delta)
		{
			index += delta;
			return *this;
		}
		iterator<u>& operator--()
		{
			index--;
			return *this;
		}
		iterator<u> operator--(int)
		{
			iterator<u> copy(*this);
			index--;
			return copy;
		}
		iterator<u>& operator-=(size_t delta)
		{
			index -= delta;
			return *this;
		}
	};
	iterator<t> begin()
	{
		return iterator<t>(this->start, this->data);
	}
	iterator<t> end()
	{
		return iterator<t>(this->start + this->len, this->data);
	}
};

template <typename t>
StackSegment<t>::StackSegment(stack<t> &dat): start(dat.size()), len(0), data(&dat) {}

template <typename t>
void StackSegment<t>::push(const t &data)
{
	this->len++;
	this->data->push(data);
    if(start+len !=  this->data->size())
        std::cout<<"Error pushing to segment not at top of stack\n";
}
template <typename t>
void StackSegment<t>::push(const t &&data)
{
	this->push(data);
}
template <typename t>
void StackSegment<t>::pop()
{
	if(len)
	{
		len--;
		this->data->pop();
	}
    else throw std::string("Error popping from empty stackstring");
}
template <typename t>
void StackSegment<t>::top(t&data)
{
	this->data->top(data);
}
template <typename t>
void StackSegment<t>::top(t*&data)
{
	this->data->top(data);
}
template <typename t>
bool StackSegment<t>::isEmpty()
{
	return this->start == this->data->size();
}
template <typename t>
size_t StackSegment<t>::length()
{
	return  len;
}
#endif /* STACKSEGMENT_HPP_ */
