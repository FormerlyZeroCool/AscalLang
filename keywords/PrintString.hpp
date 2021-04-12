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
private:
    ParsedStatementList params;
public:
	PrintString(AscalExecutor &runtime):
		StKeyword(runtime)
	{
		this->keyWord = "printStr";
	}

	void action(AscalFrame<double>* frame) override
	{
	    SubStrSV exp = ParsingUtil::getFollowingExprSV(frame->exp, frame->index, keyWord);
	    ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 1)
	    	throw std::string("loadStr (<object name>)");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
	    Object *obj = runtime.resolveNextExprSafe(frame, vns);
	    obj->printList(runtime.memory);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
	    }
	}
};

#endif /* KEYWORDDEF_PRINTSTRING_HPP_ */
