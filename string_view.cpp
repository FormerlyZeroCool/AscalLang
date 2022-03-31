/*
 * stringview.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#include "string_view.hpp"
#include "SubStr.hpp"
#include "MemoryMap.hpp"

string_view::string_view():ptr(nullptr), len(0) {}


string_view& string_view::operator=(const std::string &s)
{
	this->ptr = (char*) s.c_str();
	this->len = s.size();
	return *this;
}
string_view& string_view::operator=(const SubStrSV &s)
{
	return *this = s.data;
}
string_view::string_view(const std::string &s)
{
    this->ptr = (char*) s.c_str();
    this->len = s.size();
}
string_view::string_view(const std::string &s, const int_fast32_t start, const int_fast32_t end)
{
	this->ptr = (char*) s.c_str()+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(string_view s, const int_fast32_t start, const int_fast32_t end)
{
	this->ptr = (char*) s.ptr+start;
	this->len = (end - start)<s.size()?(end-start):s.size()-start;
}
string_view::string_view(const char *s, const int_fast32_t len)
{
	ptr = (char*) s;
	this->len = len;
}
//given an original string, and a string to search for return index of start of match, and -1 if none present
int findText2(const char *original, const int originalLen, const char *lookup, const int lookupLen)
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
int_fast32_t findText(const char *original, const int_fast32_t originalLen, const char *lookup, const int_fast32_t lookupLen)
 {
    for (int_fast32_t i = 0; i + lookupLen <= originalLen; i++) {
		if(memcmp(original + i, lookup, lookupLen) == 0)
		{
			return i;
		}
    }
    
    return -1;
}

int_fast32_t string_view::find(const std::string &s) const
{
	int_fast32_t result = findText(ptr, len, s.c_str(), (int_fast32_t) s.size());
	return result;
}
int_fast32_t string_view::find(const char *s, const int_fast32_t start, int_fast32_t size) const
{
	if(size == -1)
		size = strlen(s);
	int_fast32_t result = findText(ptr+start, len-start, s, size);
	result += (result != -1)*start;
	return result;
}
int_fast32_t string_view::find(const string_view s, const int_fast32_t start, int_fast32_t size) const
{
	if(size == -1)
		size = s.size();
	int_fast32_t result = findText(ptr+start, len-start, s.ptr, size);
	result += (result != -1)*start;
	return result;
}
int_fast32_t string_view::find(const char s, const int_fast32_t start) const
{
	int_fast32_t result = findText(ptr+start, len-start, &s, 1);
	result += (result != -1)*start;
	return result;
}
string_view string_view::substr(const int_fast32_t start, int_fast32_t len) const
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
	return this->length() == s.length() && memcmp(this->c_str(), s.c_str(), this->length()) == 0;
}

bool string_view::operator!=(const string_view &s) const
{
	return !(*this == s);
}

bool string_view::operator>(const string_view &s) const
{
	int_fast32_t result = memcmp(this->c_str(), s.c_str(), std::min(this->length(), s.length()));
    return result + (this->length() < s.length()) > 0;
}
bool string_view::operator<(const string_view &s) const
{
    
	int_fast32_t result = memcmp(this->c_str(), s.c_str(), std::min(this->length(), s.length()));
    return result - (this->length() > s.length()) < 0;
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
	for(int_fast32_t i = 0; i < l.len; i++)
		o<<l[i];
	return o;
}

StackString& StackString::operator=(const SubStrSV &s)
{
	this->copy(s.data.c_str(), s.data.size());
	return *this;
}
