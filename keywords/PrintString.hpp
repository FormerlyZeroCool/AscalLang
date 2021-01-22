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
class PrintString: public Keyword {
public:
	PrintString(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
		Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "printStr";
	}

	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 1)
	    	throw std::string("loadStr (<object name>)");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *obj = runtime->resolveNextExprSafe(frame, vns);
	    obj->printList(*memory);
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
	    }
	    return MAX;
	}
};

#endif /* KEYWORDDEF_PRINTSTRING_HPP_ */
