/*
 * LenAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_LENACTION_HPP_
#define KEYWORDS_LENACTION_HPP_

#include "../Keyword.hpp"
class LenAction: public Keyword {
public:
	LenAction(AscalExecutor &runtime):
	Keyword(runtime)
	{
		this->keyWord = "arrLen";
	}

	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    SubStr objname = ParsingUtil::getVarName(exp.data, 0);
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
