/*
 * stringview.hpp
 *
 *  Created on: Jan 6, 2021
 *      Author: andrewrubinstein
 */

#ifndef STRINGVIEW_HPP_
#define STRINGVIEW_HPP_
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "StackSegment.hpp"
struct SubStrSV;
#if defined(WIN32) || defined(_WIN32)
#include <cstring>
#else
#include <string.h>
#endif
//class ObjectIDView;
class string_view {
protected:
    char *ptr = nullptr;
    int_fast32_t len = 0;
public:
    string_view();
    string_view(const std::string &s);
//    string_view(const ObjectIDView &s);
    string_view(const std::string &s, const int_fast32_t start, const int_fast32_t end);
    string_view(const string_view s, const int_fast32_t start, const int_fast32_t end);
    string_view(const char *s, const int_fast32_t len);
    const char* c_str() const { return ptr; }
    void resize(int_fast32_t newLen)
    {
        len = newLen;
    }
    int_fast32_t size() const
    {
        return this->len;
    }
    int_fast32_t length() const
    {
        return this->size();
    }
    char& operator[](int_fast32_t index) const
    {
        return ptr[index];
    }
    std::string operator+(const string_view &s) const;
    std::string operator+(const std::string &s) const;
    string_view& operator=(const std::string &s);
    string_view& operator=(const SubStrSV &s);
    bool operator==(const string_view &s) const;
    bool operator!=(const string_view &s) const;
    bool operator<(const string_view &s) const;
    bool operator>(const string_view &s) const;
    int_fast32_t find(const std::string &s) const;
    int_fast32_t find(const char *s, int_fast32_t start = 0, int_fast32_t size = -1) const;
    int_fast32_t find(const string_view, const int_fast32_t start = 0, int_fast32_t size = -1) const;
    int_fast32_t find(const char, const int_fast32_t start = 0) const;
    string_view substr(const int_fast32_t start, int_fast32_t len = -1) const;
    std::string str() const;
    friend std::ostream& operator<<(std::ostream &o, const string_view &l);
};
using sv = string_view;
template <> struct std::hash<sv>
{
    //fnv-1a
  size_t operator()(const sv &x) const
  {
      static const uint64_t prime = 0x100000001b3, offset = 0xcbf29ce484222325;
      size_t hash = offset;
      for(int_fast32_t i = 0; i < x.length(); i++)
      {
          hash ^= (x[i]);
          hash *= prime;
      }
      return hash;
  }
};
template <> struct std::less<sv>
{
  size_t operator()(const sv &x, const sv &y) const
  {
      return x<y;
  }
};

template <> struct std::equal_to<sv>
{
  size_t operator()(const sv &x, const sv &y) const
  {
      return x == y;
  }
};

class StackString: public string_view {
private:
    size_t start;
    StackSegment<char> data;
    void loadThis(size_t len)
    {
        for(int_fast32_t i = 0; i < len; i++)
        {
            data.push((*this)[i]);
        }
        this->ptr = &data[0];
        this->len = (int_fast32_t) len;
        this->data.push(0);
    }
    inline int_fast32_t min(int_fast32_t a, int_fast32_t b)
    {
        return a<b?a:b;
    }
    inline void push(char c)
    {
        data.push(c);
        len++;
    }
    void copy(const char *ptr, int_fast32_t len)
    {
        int_fast32_t i = min(this->size(), len);
        memcpy(this->ptr, ptr, i);
        while(i < this->size() && this->size() >= len)
        {
            this->data.pop();
            i++;
        }
        while(i >= this->size() && i < len)
        {
            this->push(ptr[i++]);
        }
        this->push(0);
        this->ptr = &this->data[0];
    }
public:
    StackString(): string_view((char*)nullptr, 0), start(0) {}
    StackString(stack<char> data, const std::string &s): start(data.size()), data(data) { loadThis(s.size()); }
    
    StackString(stack<char> data, const std::string &s, const int_fast32_t start, const int_fast32_t end):
        string_view(s, start, end), start(data.size()), data(data) { loadThis(this->size()); }
    
    StackString(stack<char> data, const string_view s, const int_fast32_t start, const int_fast32_t end):
        string_view(s, start, end), start(data.size()), data(data) { loadThis(this->size()); }
    
    void setMemory(stack<char> &d) { data.setData(d);
        this->data.push(0);
        this->ptr = &this->data[0];
        this->len++;
    }
    StackString& operator=(const std::string &s) {
        this->copy(s.c_str(), s.size());
        return *this;
    }
    StackString& operator=(const std::vector<unsigned char> &s) {
        this->copy((const char*) &s[0], s.size());
        return *this;
    }
    StackString& operator=(const string_view &s)
    {
        this->copy(s.c_str(), s.size());
        return *this;
    }
    StackString& operator=(const SubStrSV &s);
};
#endif /* STRINGVIEW_HPP_ */
