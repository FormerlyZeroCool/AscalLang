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
	LenAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arrLen";
	}

	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    SubStr objname = ParsingUtil::getVarName(exp.data, 0);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size());
	    Object *obj = runtime->resolveNextExprSafe(frame, vns);
	    frame->initialOperands.push(obj->getListSize());
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"arrLen(&"<<obj->id<<") = "<<obj->getListSize()<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_LENACTION_HPP_ */
