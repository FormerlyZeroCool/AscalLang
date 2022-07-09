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
	stack<t> *data;
	uint32_t start;
public:
    //only used while returning value from function into previous function's operand stack
    inline void resetStart() noexcept
    {
        this->start = this->data->size();
    }
    inline void setStart(uint32_t newStart) noexcept
    {
        this->start = newStart;
    }
	inline void setData(stack<t> &dat) noexcept
	{
		data = &dat;
		start = dat.size();
	}
	void clear() noexcept
	{
        /*if(start+len == data->size() || !len){
            for(size_t i = 0; i < len; i++)
            this->data->pop();
        }
        else if(!data->isEmpty()){
            std::cout<<(start+len)<<" "<<data->size()<<("Error clearing segment not at top of stack\n");
        }*/
        if(start < this->data->size())
            this->data->erase(this->data->begin() + start, this->data->end());
	}
	~StackSegment()
	{
		clear();
	}
	StackSegment(stack<t> &dat);
	StackSegment() noexcept {start = 0; data = nullptr;}
	inline t& operator[](size_t index) const noexcept
	{
        //if(index < len)
            return (*data)[index+start];
        //else
          //  throw std::string("Invalid access "+std::to_string(index));
	}
	inline void push(const t &data) noexcept;
	inline void push(const t &&data) noexcept;
    inline void push_back(const t &data) noexcept
	{
		push(data);
	}
	inline void push_back(const t &&data) noexcept
	{
		push(data);
	}
	inline void pop() noexcept;
    inline void pop_back() noexcept { pop(); }
	inline void top(t&data) const noexcept;
	inline void top(t*&data) const noexcept;
	inline bool isEmpty() const noexcept;
	inline size_t length() const noexcept;
	inline size_t size() const noexcept { return length(); }

	template <typename u>
	class iterator {
	private:
		size_t index;
		stack<u> *data;
	public:
		iterator(size_t index, stack<u> *data): index(index),data(data) {}
		bool operator==(iterator<u> &o) const
		{
			return this->data == o.data &&
					this->index == o.index;
		}
		bool operator!=(iterator<u> &o) const
		{
			return this->index != o.index ||
					this->data != o.data;
		}
		u& operator*() const
		{
			return (*data)[index];
		}
		u& operator->() const
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
	iterator<t> begin() const
	{
		return iterator<t>(this->start, this->data);
	}
	iterator<t> end() const
	{
		return iterator<t>(this->data->size(), this->data);
	}
};

template <typename t>
StackSegment<t>::StackSegment(stack<t> &dat): data(&dat), start(dat.size()) {}

template <typename t>
void StackSegment<t>::push(const t &data) noexcept
{
	this->data->push(data);
    //if(start+len !=  this->data->size())
      //  std::cout<<"Error pushing to segment not at top of stack\n";
}
template <typename t>
void StackSegment<t>::push(const t &&data) noexcept
{
	this->push(data);
}
template <typename t>
void StackSegment<t>::pop() noexcept
{
		this->data->pop();
}
template <typename t>
void StackSegment<t>::top(t&data) const  noexcept
{
	this->data->top(data);
}
template <typename t>
void StackSegment<t>::top(t*&data) const noexcept
{
	this->data->top(data);
}
template <typename t>
bool StackSegment<t>::isEmpty() const noexcept
{
	return this->start >= this->data->size();
}
template <typename t>
size_t StackSegment<t>::length() const noexcept
{
	return  this->data->size() - this->start;
}
#endif /* STACKSEGMENT_HPP_ */
