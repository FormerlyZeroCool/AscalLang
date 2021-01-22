/*
 * Keyword.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */

#include "Keyword.hpp"
#include "AscalExecutor.hpp"

Keyword::Keyword(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings) :
runtime(runtime), boolsettings(boolsettings), memory(memory){

}

const std::string Keyword::MAX = std::to_string(std::numeric_limits<double>::max());
Keyword::~Keyword() {}

