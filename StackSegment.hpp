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
	void setData(stack<t> &dat)
	{
		data = &dat;
		start = dat.size();
	}
	void clear()
	{
        const size_t length = len;
		for(size_t i = 0; i < length; i++)
            this->pop();
	}
	~StackSegment()
	{
		clear();
	}
	StackSegment(stack<t> &dat);
	StackSegment() {start = 0; len = 0; data = nullptr;}
	t& operator[](size_t index)
	{
		return (*data)[index+start];
	}
	void push(const t &data);
	void push(const t &&data);
	void push_back(const t &data)
	{
		push(data);
	}
	void push_back(const t &&data)
	{
		push(data);
	}
	void pop();
    void pop_back() { pop(); }
	void top(t&data);
	void top(t*&data);
	bool isEmpty();
	size_t length();
	size_t size() { return length(); }

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
			t &d = (*data)[index];
			return d;
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
	return  this->data->size() - this->start;
}
#endif /* STACKSEGMENT_HPP_ */
