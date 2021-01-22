/*
 * LoadStrAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_LOADSTRACTION_HPP_
#define KEYWORDS_LOADSTRACTION_HPP_

#include "../Keyword.hpp"
class LoadStrAction: public Keyword {
public:
	LoadStrAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "loadStr";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 2)
	    	throw std::string("loadStr (<Object name>, <\"string in quotes\">");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *obj = runtime->resolveNextExprSafe(frame, vns);
	    obj->clearList();
	    //parse only text between quotes
	    size_t strStart = 0;
	    while(params[1].data[strStart] && params[1].data[strStart++] != '\"'){}
	    size_t strEnd = params[1].data.length()-1;
	    while(params[1].data[strEnd] && params[1].data[strEnd--] != '\"'){}
	    //at this point str start, and end point to the appropriate places in the parameter
	    string_view s((char*)params[1].data.c_str()+strStart, strEnd);
	    obj->loadString(s);

	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
	    }
	    return MAX;
	}
};

#endif /* KEYWORDS_LOADSTRACTION_HPP_ */
