/*
 * SplitStrAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SPLITSTRACTION_HPP_
#define KEYWORDS_SPLITSTRACTION_HPP_
#include "../Keyword.hpp"
class SplitStrAction: public Keyword {
public:
	SplitStrAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "splitStr";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr > params = Object("","",exp.data).params;
	    if(params.size() < 2)
	    	throw std::string("splitStr (<object name>, <\"delimiter\">");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *obj = runtime->resolveNextExprSafe(frame, vns);
	    //parse only text between quotes
	    size_t strStart = 0;
	    while(params[1].data[strStart] && params[1].data[strStart++] != '\"'){}
	    size_t strEnd = params[1].data.length()-1;
	    while(params[1].data[strEnd] && params[1].data[strEnd--] != '\"'){}

	    string_view filter((char*) params[1].data.c_str()+strStart, strEnd-1);
	    //at this point string start, and end point to the appropriate places in the parameter
	    Object arr = obj->splitString(filter, *memory);
	    (*frame->getLocalMemory())[arr.id] = arr;
        if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"split string("<<arr.id<<") added to memory\n";
	    }
	    return MAX;
	}
};

#endif /* KEYWORDS_SPLITSTRACTION_HPP_ */
