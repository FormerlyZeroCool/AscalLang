/*
 * stringview.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#include "string_view.hpp"
#include "SubStr.hpp"
#include "MemoryMap.hpp"

string_view::string_view() {}

string_view::~string_view() {}

//string_view::string_view(const ObjectIDView &s): string_view(s.str()) {}
string_view& string_view::operator=(const std::string &s)
{
	this->ptr = (char*) s.c_str();
	this->len = s.size();
	return *this;
}/*
string_view& string_view::operator=(const string_view &s)
{
    this->ptr = s.ptr;
    this->len = s.size();
    return *this;
}
string_view& string_view::operator=(const string_view &&s)
{
    *this = s;
    return *this;
}*/
string_view& string_view::operator=(const SubStrSV &s)
{
	return *this = s.data;
}
string_view::string_view(const std::string &s)
{
    this->ptr = (char*) s.c_str();
    this->len = s.size();
}
string_view::string_view(const std::string &s, const uint32_t start, const uint32_t end)
{
	this->ptr = (char*) s.c_str()+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(string_view s, const uint32_t start, const uint32_t end)
{
	this->ptr = (char*) s.ptr+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(const char *s, const uint32_t len)
{
	ptr = (char*) s;
	this->len = len;
}
//given an original string, and a string to search for return index of start of match, and -1 if none present
uint32_t findText(const char *original, const uint32_t originalLen, const char *lookup, const uint32_t lookupLen)
{
    if(!lookupLen || !originalLen || lookupLen>originalLen || !original || !lookup)
    {
        return -1;
    }
    bool result = 0;    
	//i is the current index in the original that could be the start of a match
    //j is the current index into the possible match starts at 0 to index lookup string
    uint32_t i = 0, j = 0;
    while(!result & originalLen > i)
    {
        //j ^= j sets j = 0;
        j ^= j;
		result = original[i] == lookup[j]; 
        while(result & lookupLen > j & originalLen > i+j)
        {
            result = original[i+j] == lookup[j];
            j++;
        }
        result = ((j==lookupLen) & result);
        i++;
    }
    i = result*(i-1) - !result;
    return i;
}

uint32_t string_view::find(const std::string &s) const
{
	uint32_t result = findText(ptr, len, s.c_str(), (uint32_t) s.size());
	return result;
}
uint32_t string_view::find(const char *s, const uint32_t start, uint32_t size) const
{
	if(size == -1)
		size = strlen(s);
	uint32_t result = findText(ptr+start, len-start, s, size);
	result += (result != -1)*start;
	return result;
}
uint32_t string_view::find(const string_view s, const uint32_t start, uint32_t size) const
{
	if(size == -1)
		size = s.size();
	uint32_t result = findText(ptr+start, len-start, s.ptr, size);
	result += (result != -1)*start;
	return result;
}
uint32_t string_view::find(const char s, const uint32_t start) const
{
	uint32_t result = findText(ptr+start, len-start, &s, 1);
	result += (result != -1)*start;
	return result;
}
string_view string_view::substr(const uint32_t start, uint32_t len) const
{
	if(len==-1)
		len = this->len;
	return string_view(*this, start, len+start);
}
std::string string_view::str() const
{
	return std::string(ptr, len);
}

bool string_view::operator==(const string_view &s) const
{
	bool equal = this->length() == s.length();
	for(uint32_t i = 0; i < this->length() && equal; i++)
		equal = s[i] == (*this)[i];
	return equal;
}

bool string_view::operator<(const string_view &s) const
{
    const char *s1 = this->c_str();
    const char *s2 = s.c_str();
    const char s1t = (*this)[this->size()];
    const char s2t = (s)[s.size()];
    (*this)[this->size()] = 0;
    s[s.size()] = 0;
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    const bool result = (*(const unsigned char*)s1 - *(const unsigned char*)s2)<0;
    (*this)[this->size()] = s1t;
    s[s.size()] = s2t;
    return result;/*
    bool isLessThan = false;
	bool checking = true;
	uint32_t i = 0;
    uint32_t max = std::min(s.length(), (*this).length());
	while(!isLessThan && checking && i < max)
	{
	    if((*this)[i] < s[i])
	        isLessThan = true;
	    else if((*this)[i] > s[i])
	        checking = false;
	    i++;
	}
	isLessThan = isLessThan || (checking && s.length() > (*this).length());

	return isLessThan;*/
}

std::string string_view::operator+(const string_view &s) const
{
	std::stringstream stream;
	stream<<*this;
	stream<<s;
	return stream.str();
}
std::string string_view::operator+(const std::string &s) const
{
	std::stringstream stream;
	stream<<*this;
	stream<<s;
	return stream.str();
}
std::ostream& operator<<(std::ostream &o,const string_view &l)
{
	for(uint32_t i = 0; i < l.len; i++)
		o<<l[i];
	return o;
}

StackString& StackString::operator=(const SubStrSV &s)
{
	this->copy(s.data.c_str(), s.data.size());
	return *this;
}
