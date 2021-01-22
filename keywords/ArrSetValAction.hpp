/*
 * ArrSetValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRSETVALACTION_HPP_
#define KEYWORDS_ARRSETVALACTION_HPP_

#include "../Keyword.hpp"
class ArrSetValAction: public Keyword {
public:
	ArrSetValAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arrSet";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 3)
	    	throw std::string("arrGet(<array>,<Index as Ascal expression>,<value as expression>)");
	    double indexToSet = runtime->callOnFrame(frame,params[1].data);
	    double value = runtime->callOnFrame(frame,params[2].data);
	    Object newObj("",ParsingUtil::to_string(value),"");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    runtime->resolveNextExprSafe(frame, vns)->setList(newObj, indexToSet);
	    (*memory)["null"].getInstructions() = MAX;
	    if(*(*boolsettings)["o"])
	    {
	    	double t;
	    	frame->initialOperands.top(t);
	    	std::cout<<"got element at index: "<<params[1].data<<" from list "<<params[0].data<<" value: "<<(t)<<"\n";
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_ARRSETVALACTION_HPP_ */
