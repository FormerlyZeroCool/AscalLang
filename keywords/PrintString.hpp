/*
 * PrintString.hpp
 *
 *  Created on: Dec 23, 2020
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDDEF_PRINTSTRING_HPP_
#define KEYWORDDEF_PRINTSTRING_HPP_
#include "../Keyword.hpp"
#include "../AscalExecutor.hpp"
#include "../ParsingUtil.hpp"
class PrintString: public StKeyword {
public:
	PrintString(AscalExecutor &runtime):
		StKeyword(runtime)
	{
		this->keyWord = "printStr";
	}

	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    std::vector<SubStrSV> params = Object(runtime.memMan, "","",exp.data).params;
	    if(params.size() < 1)
	    	throw std::string("loadStr (<object name>)");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *obj = runtime.resolveNextExprSafe(frame, vns);
	    obj->printList(runtime.memory);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
	    }
	}
};

#endif /* KEYWORDDEF_PRINTSTRING_HPP_ */
