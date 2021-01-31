/*
 * stringview.hpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#ifndef STRINGVIEW_HPP_
#define STRINGVIEW_HPP_
#include <string>
#include <iostream>
#if defined(WIN32) || defined(_WIN32)
#include <cstring>
#else
#include <string.h>
#endif
class string_view {
private:
	char *ptr = nullptr;
	uint32_t len = 0;
public:
	string_view();
	string_view(const std::string &s);
	string_view(const std::string &s, uint32_t start, uint32_t end);
	string_view(const string_view &s, uint32_t start, uint32_t end);
	string_view(const char *s, uint32_t len);
	uint32_t size()
	{
		return this->len;
	}
	uint32_t length()
	{
		return this->size();
	}
	char& operator[](uint32_t index)
	{
		return ptr[index];
	}
	uint32_t find(const std::string &s);
	uint32_t find(const char *s, uint32_t start = 0, uint32_t size = -1);
	uint32_t find(string_view, uint32_t start = 0, uint32_t size = -1);
	string_view substr(uint32_t start, uint32_t end = -1);
	std::string str();
	friend std::ostream& operator<<(std::ostream &o,string_view &l);
	friend std::ostream& operator<<(std::ostream &o,string_view &&l);
	virtual ~string_view();
};

#endif /* STRINGVIEW_HPP_ */
