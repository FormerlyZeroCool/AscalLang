//
//  ParseMap.hpp
//  AscalLang
//
//  Created by Andrew Rubinstein on 4/4/21.
//  Copyright © 2021 Andrew Rubinstein. All rights reserved.
//

#ifndef ParseMap_hpp
#define ParseMap_hpp

#include <iostream>
#include <map>
#include "Object.hpp"
#include "MemoryManager.hpp"
#include "AscalExecutor.hpp"
#include "StackSegment.hpp"

class ParseMap {
private:
    StackSegment<Object* > data;
    std::map<string_view, uint32_t > map;
public:
    ParseMap(stack<Object* > &stack): data(stack){}
    void push(string_view id, string_view instructions);
    void pop();
    Object& operator[](uint32_t);
    uint32_t operator[](string_view);
};


#endif /* ParseMap_hpp */
