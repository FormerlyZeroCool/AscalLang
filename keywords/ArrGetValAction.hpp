/*
 * ArrGetValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRGETVALACTION_HPP_
#define KEYWORDS_ARRGETVALACTION_HPP_

#include "../Keyword.hpp"
class ArrGetValAction: public Keyword {
public:
	ArrGetValAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arrGet";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 2)
	    	throw std::string("arrGet(<array>,<Index as Ascal expression>)");
	    double indexToGet = runtime->callOnFrame(frame,params[1].data);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *element = &runtime->resolveNextExprSafe(frame, vns)->getListElement(indexToGet, *memory);
	    SubStr paramsForListElementFn("", 0 , exp.end);
	    if(exp.data[exp.end] == '(')
	    {
	    	frame->index = exp.end;
	    	paramsForListElementFn = ParsingUtil::getFollowingExpr(frame->exp, frame->index, std::string(""));
	    }
	    std::stringstream fnDef;
	    fnDef<<"loc "<<element->id<<"="<<element->getInstructions()<<";\n"<<element->id<<'('<<paramsForListElementFn.data<<')';

	    frame->initialOperands.push(runtime->callOnFrame(frame,fnDef.str()));

	    runtime->callOnFrame(frame, "delete "+element->id);
	    if(*(*boolsettings)["o"])
	    {
	    	double t;
	    	frame->initialOperands.top(t);
	    	std::cout<<"got element at index: "<<params[1].data<<" from list "<<params[0].data<<" value: "<<(t)<<"\n";
	    }
	    return 'a'+frame->exp.substr(paramsForListElementFn.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_ARRGETVALACTION_HPP_ */
