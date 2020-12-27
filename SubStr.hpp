/*
 * SubStr.hpp
 *
 *  Created on: Dec 26, 2020
 *      Author: andrewrubinstein
 */

#ifndef SUBSTR_HPP_
#define SUBSTR_HPP_
#include <string>
struct SubStr{
    std::string data;
    int start,end;
    bool loadedNew = false;
    SubStr(std::string data,int start,int end):data(data),start(start),end(end){}
};

#endif /* SUBSTR_HPP_ */
