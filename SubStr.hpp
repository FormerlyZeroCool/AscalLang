/*
 * SubStr.hpp
 *
 *  Created on: Dec 26, 2020
 *      Author: andrewrubinstein
 */

#ifndef SUBSTR_HPP_
#define SUBSTR_HPP_
#include <string>
#include "string_view.hpp"
struct SubStrSV;
struct SubStr{
    std::string data;
    int start,end;
    bool loadedNew = false;
    SubStr(std::string data,int start,int end):data(data),start(start),end(end){}
    SubStr(string_view data,int start,int end):start(start),end(end){ this->data = data.str(); }
    SubStr(SubStrSV s);
};
struct SubStrSV {
    string_view data;
    int start,end;
    SubStrSV& operator=(const SubStr& s)
    {
    	data = s.data;
    	start = s.start;
    	end = s.end;
    	return *this;
    }
    SubStrSV(const SubStr &s){ *this = s; }
    SubStrSV(const string_view &s){ this->data = s; this->start = 0; this->end = s.length(); }
    SubStrSV(string_view data,int start,int end):data(data),start(start),end(end){}
};

#endif /* SUBSTR_HPP_ */
