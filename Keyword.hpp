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
	AscalExecutor &runtime;
public:
	Keyword(AscalExecutor &runtime);
	const std::string& getName() { return keyWord; }
	void setName(std::string key) { keyWord = key; }
	virtual void action(AscalFrame<double> *frame) {}
	virtual uint8_t getType() { return 'r'; }
	virtual ~Keyword();
};
class OpKeyword : public Keyword {
public:
	OpKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual uint8_t getType() override { return 1; }

};
class StKeyword : public Keyword {
public:
	StKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual uint8_t getType() override { return 0; }
};
class ComplexStKeyword : public Keyword {
public:
	ComplexStKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual uint8_t getType() override { return 3; }
};
#endif /* KEYWORD_HPP_ */
