/*
 * svo.h
 *
 *  Created on: May 4, 2020
 *      Author: andrew
 */

#ifndef SVO_H_
#define SVO_H_
#include <vector>
template <typename type,int bufferSize>
class Svo{
private:
	std::vector<type> largeData;
	type smallData[bufferSize];
	int len;

public:
	int size()
	{
		return len;
	}

	Svo():len(0){}
	type& operator[](int index)
	{
		type result;
		if(index < bufferSize)
			result = smallData[index];
		else
			result = largeData[index-bufferSize];
		return result;
	}
	void push_back(const type &data)
	{
		if(len < bufferSize)
		{
			smallData[len] = data;
		}
		else
		{
			largeData.push_back(data);
		}
		len++;
	}
	void pop_back()
	{
		if(len >= bufferSize)
		{
			largeData.pop_back();
		}
		len--;
	}
	void reserve(int size){
		largeData.reserve(size);
	}
	class Iterator{
	private:
		std::vector<type>* largeData;
		type* smallData;
		int index;
 	public:

        typedef Iterator self_type;
        typedef type value_type;
        typedef type& reference;
        typedef type* pointer;
        typedef std::input_iterator_tag iterator_category;
        typedef int difference_type;
		struct input_iterator_tag {};
		Iterator(int index,type* smallData,std::vector<type>* largeData):largeData(largeData),index(index)
		{
			this->smallData = smallData;
		}
		type& operator=(type& data)
		{
			this->index = index;
			if(index < bufferSize)
				smallData[index] = data;
			else
				(*largeData)[index-bufferSize] = data;
			return *this;
		}
		Iterator& operator=(Iterator& data)
		{
			this->index = index;
			if(index < bufferSize)
				smallData[index] = *data;
			else
				(*largeData)[index-bufferSize] = *data;
			return *this;
		}
		type& operator[](int index)
		{
			type* result;
			this->index = index;
			if(index < bufferSize)
				result = &smallData[index];
			else
				result = &(*largeData)[index-bufferSize];
			return *result;
		}
		type& operator*()
		{
			type* data;
			if(index < bufferSize)
				data = &smallData[index];
			else
				data = &(*largeData)[index-bufferSize];
			return *data;
		}
		type& operator->()
		{
			type* result;
			this->index = index;
			if(index < bufferSize)
				result = &smallData[index];
			else
				result = &(*largeData)[index-bufferSize];
			return *result;
		}
		Iterator& operator++()
		{
			++index;
			return *this;
		}
		Iterator operator++(int)
		{
			Iterator data = *this;
			++index;
			return data;
		}
		Iterator& operator--()
		{
			--index;
			return *this;
		}
		Iterator operator--(int)
		{
			Iterator data = *this;
			--index;
			return data;
		}
		bool operator==(const type &data)
		{
			bool result;
			if(index < bufferSize)
			{
				result = data == smallData[index];
			}
			else
				result = data == (*largeData)[index-bufferSize];
			return result;
		}
		bool operator!=(const type &data)
		{
			bool result;
			if(index < bufferSize)
			{
				result = data != smallData[index];
			}
			else
				result = data != (*largeData)[index-bufferSize];
			return result;
		}
		bool operator==(Iterator &data)
		{
			bool result;
			if(index < bufferSize)
			{
				result = data == smallData[index];
			}
			else
				result = data == (*largeData)[index-bufferSize];
			return result;
		}
		bool operator!=(Iterator &data)
		{
			bool result;
			if(index < bufferSize)
			{
				result = data != smallData[index];
			}
			else
				result = data != (*largeData)[index-bufferSize];
			return result;
		}
	};
	Iterator begin() { return Iterator(0,smallData,&largeData); }
	Iterator end() { return Iterator(len,smallData,&largeData); }
	Iterator rbegin() { return Iterator(len-1,smallData,&largeData); }
	Iterator rend() { return Iterator(-1,smallData,&largeData); }
};



#endif /* SVO_H_ */
