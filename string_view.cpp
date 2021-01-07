/*
 * stringview.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#include "string_view.hpp"

string_view::string_view() {}

string_view::~string_view() {}

string_view::string_view(std::string &s)
{
	this->ptr = (char*) s.c_str();
	this->len = s.size();
}
string_view::string_view(std::string &s, uint32_t start, uint32_t end)
{
	this->ptr = (char*) s.c_str()+start;
	this->len = end - start;
}
string_view::string_view(char *s, uint32_t len)
{
	ptr = s;
	this->len = len;
}
uint32_t string_view::size()
{
	return this->len;
}
uint32_t string_view::length()
{
	return this->size();
}
uint32_t findText(char *original, uint32_t olen, char *lookup, uint32_t llen)
{
    if(!llen)
        return 0;
    bool intRes = 1, result = 0;
    uint32_t i = 0, j = 0;
    while(!result && olen > i)
    {
        if(original[i] == lookup[0])
        {
            while(intRes && llen > j && olen > i+j)
            {
                if(original[i+j] != lookup[j])
                    intRes = 0;
                j++;
            }
            result = intRes;
            intRes = !intRes;
        }
        i++;
    }
    i = result*(i-1) + !result*(-1);
    return i;
}
uint32_t string_view::find(std::string &s)
{
	const char *intArray = s.c_str();
	return findText(ptr, len, (char*) intArray, (uint32_t) s.size());
}
uint32_t string_view::find(char *s, uint32_t start, uint32_t size)
{
	if(size == -1)
		size = strlen(s);
	return findText(ptr, len, s+start, size);
}
uint32_t string_view::find(string_view s, uint32_t start, uint32_t size)
{
	if(size == -1)
		size = s.size();
	return findText(ptr, len, s.ptr+start, size);
}
char& string_view::operator[](uint32_t index)
{
	return ptr[index];
}
std::string string_view::str()
{
	return std::string(ptr, len);
}

