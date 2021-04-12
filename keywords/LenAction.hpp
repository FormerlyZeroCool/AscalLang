/*
 * LenAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_LENACTION_HPP_
#define KEYWORDS_LENACTION_HPP_

#include "../Keyword.hpp"
class LenAction: public OpKeyword {
public:
	LenAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "arrLen";
	}

	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExprSV(frame->exp, frame->index, keyWord);
        uint32_t index = frame->index+this->keyWord.size();
	    SubStr objname = ParsingUtil::getVarNameSV(exp.data, index);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size());
	    Object *obj = runtime.resolveNextExprSafe(frame, vns);
	    frame->initialOperands.push(obj->getListSize());
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"arrLen(&"<<obj->getId()<<") = "<<obj->getListSize()<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_LENACTION_HPP_ */
