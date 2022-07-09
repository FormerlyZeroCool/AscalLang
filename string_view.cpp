/*
 * stringview.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#include "string_view.hpp"
#include "SubStr.hpp"

string_view::string_view() noexcept: ptr(nullptr), len(0)  {}  


string_view& string_view::operator=(const std::string &s) noexcept
{
	this->ptr = (char*) s.c_str();
	this->len = s.size();
	return *this;
}
string_view& string_view::operator=(const SubStrSV &s) noexcept
{
	return *this = s.data;
}
string_view::string_view(const std::string &s) noexcept
{
    this->ptr = (char*) s.c_str();
    this->len = s.size();
}
string_view::string_view(const std::string &s, const uint32_t start, const uint32_t end) noexcept
{
	this->ptr = (char*) s.c_str()+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(string_view s, const uint32_t start, const uint32_t end) noexcept
{
	this->ptr = (char*) s.ptr+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(const char *s, const uint32_t len) noexcept
{
	ptr = (char*) s;
	this->len = len;
}
//given an original string, and a string to search for return index of start of match, and -1 if none present
int findText2(const char *original, const int originalLen, const char *lookup, const int lookupLen) noexcept
 {
    for (int i = 0; i < originalLen; i++) {
        for (int j = 0; j < lookupLen; j++) {
            if (original[i+j] != lookup[j])
                goto breakLookupLoop;
        }
        
        return i;
        breakLookupLoop:;
    }
    
    return -1;
}
//given an original string, and a string to search for return index of start of match, and -1 if none present
uint32_t findText(const char *original, const uint32_t originalLen, const char *lookup, const uint32_t lookupLen) noexcept
 {
    for (uint32_t i = 0; i + lookupLen <= originalLen; i++) {
		if(memcmp(original + i, lookup, lookupLen) == 0)
		{
			return i;
		}
    }
    
    return -1;
}

uint32_t string_view::find(const std::string &s) const noexcept
{
	uint32_t result = findText(ptr, len, s.c_str(), (uint32_t) s.size());
	return result;
}
uint32_t string_view::find(const char *s, const uint32_t start, uint32_t size) const noexcept
{
	if(size == -1)
		size = strlen(s);
	uint32_t result = findText(ptr+start, len-start, s, size);
	result += (result != -1)*start;
	return result;
}
uint32_t string_view::find(const string_view s, const uint32_t start, uint32_t size) const noexcept
{
	if(size == -1)
		size = s.size();
	uint32_t result = findText(ptr+start, len-start, s.ptr, size);
	result += (result != -1)*start;
	return result;
}
uint32_t string_view::find(const char s, const uint32_t start) const noexcept
{
	uint32_t result = findText(ptr+start, len-start, &s, 1);
	result += (result != -1)*start;
	return result;
}
string_view string_view::substr(const uint32_t start, uint32_t len) const noexcept
{
	if(len==-1)
		len = this->len;
	return string_view(*this, start, len+start);
}
std::string string_view::str() const 
{
	return std::string(ptr, len);
}

bool string_view::operator!=(const string_view &s) const noexcept
{
	return !(*this == s);
}

bool string_view::operator==(const string_view &s) const noexcept
{
	return this->length() == s.length() && memcmp(this->c_str(), s.c_str(), this->length()) == 0;
}
bool string_view::operator>(const string_view &s) const noexcept
{
	uint32_t result = memcmp(this->c_str(), s.c_str(), std::min(this->length(), s.length()));
    return result + (this->length() < s.length()) > 0;
}
bool string_view::operator<(const string_view &s) const noexcept
{
    
	uint32_t result = memcmp(this->c_str(), s.c_str(), std::min(this->length(), s.length()));
    return result - (this->length() > s.length()) < 0;
}

std::string string_view::operator+(const string_view &s) const noexcept
{
	std::stringstream stream;
	stream<<*this;
	stream<<s;
	return stream.str();
}
std::string string_view::operator+(const std::string &s) const noexcept
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
