/*
 * PushValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRPUSHVALACTION_HPP_
#define KEYWORDS_ARRPUSHVALACTION_HPP_

#include "../Keyword.hpp"
class ArrPushValAction: public Keyword {
public:
	ArrPushValAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arrPush";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 2)
	    	throw std::string("arrPush(<object/array>,<value as Ascal expression>)");
	    double valueToPush = runtime->callOnFrame(frame,params[1].data);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *list = runtime->resolveNextExprSafe(frame, vns);
	    list->pushList(Object("", ParsingUtil::to_string(valueToPush), ""));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"pushed value "<<params[1].data<<" to list "<<params[0].data<<" at position: "<<(list->getListSize())<<"\n";
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_PUSHVALACTION_HPP_ */
