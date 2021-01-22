/*
 * Keyword.hpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */

#ifndef KEYWORD_HPP_
#define KEYWORD_HPP_
#include <unordered_map>
#include <map>
#include <string>
#include "Object.hpp"
#include "setting.hpp"
#include "AscalFrame.hpp"
#include "AscalExecutor.hpp"
#include "ParsingUtil.hpp"
//create a wrapper class that has pointers to functions
class Keyword {
protected:
	static const std::string MAX;
	std::string keyWord;
	//Program Global Memory
	std::unordered_map<std::string,Object> *memory;
	//Interpreter Settings HashMap for toggle flags, like show time, or operations
	std::map<std::string,setting<bool> > *boolsettings;
	AscalExecutor *runtime;
public:
	Keyword(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings);
	std::string getName() { return keyWord; }
	void setName(std::string key) { keyWord = key; }
	virtual std::string action(AscalFrame<double> *frame) { return ""; }
	virtual ~Keyword();
};
#endif /* KEYWORD_HPP_ */
