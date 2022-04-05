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
	    while(frame->exp[index] == ' '){ index++; }
	    SubStrSV subexp = ParsingUtil::getExprInStringSV(frame->exp, index, '(', ')', ';');
	    SubStrSV varName = ParsingUtil::getVarNameSV(frame->exp, static_cast<uint32_t>(subexp.start));
	    index += varName.data.length();

	    if(runtime.getObjectNoError(frame, varName.data))
	    {
	    	frame->initialOperands.push(1);
	    }
	    else
	    {
	    	frame->initialOperands.push(0);
	    }
		frame->index = subexp.end;

	}
};

#endif /* KEYWORDS_EXISTSACTION_HPP_ */
