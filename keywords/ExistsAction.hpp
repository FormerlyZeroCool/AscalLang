/*
 * ExistsAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_EXISTSACTION_HPP_
#define KEYWORDS_EXISTSACTION_HPP_

#include "../Keyword.hpp"
class ExistsAction: public Keyword {
public:
	ExistsAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "exists";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    int index = frame->index+6;
	    while(frame->exp[index++] == ' '){}
	    SubStr varName = ParsingUtil::getVarName(frame->exp,index);
	    index += varName.data.length();

	    if(frame->getLocalMemory()->count(varName.data) || frame->getParamMemory()->count(varName.data) || memory->count(varName.data))
	    {
	    	frame->initialOperands.push(1);
	        return "a"+frame->exp.substr(index,frame->exp.size());
	    }
		frame->initialOperands.push(0);
	    return "a"+frame->exp.substr(index,frame->exp.size());
	}
};

#endif /* KEYWORDS_EXISTSACTION_HPP_ */
