/*
 * stringview.hpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#ifndef STRINGVIEW_HPP_
#define STRINGVIEW_HPP_
#include <string>
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
	string_view(std::string &s);
	string_view(std::string &s, uint32_t start, uint32_t end);
	string_view(char *s, uint32_t len);
	uint32_t size();
	uint32_t length();
	uint32_t find(std::string &s);
	uint32_t find(char *s, uint32_t start = 0, uint32_t size = -1);
	uint32_t find(string_view, uint32_t start = 0, uint32_t size = -1);
	char& operator[](uint32_t index);
	std::string str();
	virtual ~string_view();
};

#endif /* STRINGVIEW_HPP_ */
