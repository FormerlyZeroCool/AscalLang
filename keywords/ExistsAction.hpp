/*
 * ExistsAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_EXISTSACTION_HPP_
#define KEYWORDS_EXISTSACTION_HPP_

#include "../Keyword.hpp"
class ExistsAction: public OpKeyword {
public:
	ExistsAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "exists";
	}
	void action(AscalFrame<double>* frame) override
	{
	    int index = frame->index+6;
	    while(frame->exp[index++] == ' '){}
	    SubStr varName = ParsingUtil::getVarName(frame->exp,index);
	    index += varName.data.length();

	    if(frame->getLocalMemory()->count(varName.data) || frame->getParamMemory()->count(varName.data) || runtime.memory.find(varName.data) != runtime.memory.end())
	    {
	    	frame->initialOperands.push(1);
	    }
	    else
	    {
	    	frame->initialOperands.push(0);
	    }
		frame->index = index;

	}
};

#endif /* KEYWORDS_EXISTSACTION_HPP_ */
