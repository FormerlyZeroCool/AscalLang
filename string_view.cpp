/*
 * stringview.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#include "string_view.hpp"

string_view::string_view() {}

string_view::~string_view() {}

string_view::string_view(const std::string &s)
{
	this->ptr = (char*) s.c_str();
	this->len = s.size();
}
string_view::string_view(const std::string &s, uint32_t start, uint32_t end)
{
	this->ptr = (char*) s.c_str()+start;
	this->len = (end - start)<s.size()?(end-start):s.size();
}
string_view::string_view(const char *s, uint32_t len)
{
	ptr = (char*) s;
	this->len = len;
}
uint32_t findText(const char *original, uint32_t olen, const char *lookup, uint32_t llen)
{;
    if(!llen || !olen || llen>olen || !original || !lookup)
    {
        return -1;
    }
    bool intRes = 1, result = 0;
    uint32_t i = 0, j = 0;
    while(!result && olen > i)
    {
        if(original[i] == lookup[0])
        {
            j = 0;
            while(intRes && llen > j && olen > i+j)
            {
                intRes = original[i+j] == lookup[j];
                j++;
            }
            intRes = ((j==llen) && intRes);
            //j ^= j sets j = 0;
            result = intRes;
            intRes = !intRes;
        }
        i++;
    }
    i = result*(i-1) + !result*(-1);
    return i;
}
uint32_t string_view::find(const std::string &s)
{
	const char *intArray = s.c_str();
	return findText(ptr, len, (char*) intArray, (uint32_t) s.size());
}
uint32_t string_view::find(const char *s, uint32_t start, uint32_t size)
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
string_view string_view::substr(uint32_t start, uint32_t end)
{
	if(end==-1)
		end = this->len;
	return string_view(this->ptr+start, end-start);
}
std::string string_view::str()
{
	return std::string(ptr, len);
}

std::ostream& operator<<(std::ostream &o,string_view &l)
{
	for(uint32_t i = 0; i < l.len; i++)
		o<<l[i];
	return o;
}
