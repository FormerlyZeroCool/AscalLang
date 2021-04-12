/*
 * SplitStrAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SPLITSTRACTION_HPP_
#define KEYWORDS_SPLITSTRACTION_HPP_
#include "../Keyword.hpp"
class SplitStrAction: public StKeyword {
private:
    ParsedStatementList params;;
public:
	SplitStrAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "splitStr";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 2)
	    	throw std::string("splitStr (<object name>, <\"delimiter\">");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
	    Object *obj = runtime.resolveNextExprSafe(frame, vns);
	    //parse only text between quotes
	    size_t strStart = 0;
	    while(params.statements[1].data[strStart] && params.statements[1].data[strStart++] != '\"'){}
	    size_t strEnd = params.statements[1].data.length()-1;
	    while(params.statements[1].data[strEnd] && params.statements[1].data[strEnd--] != '\"'){}

	    string_view filter(&params.statements[1].data[0]+strStart, strEnd-1);
	    //at this point string start, and end point to the appropriate places in the parameter
	    Object arr = obj->splitString(filter, runtime.memory);
	    (*frame->getLocalMemory())[arr.getId()] = arr;
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"split string("<<arr.getId()<<") added to memory\n";
	    }
	}
};

#endif /* KEYWORDS_SPLITSTRACTION_HPP_ */
