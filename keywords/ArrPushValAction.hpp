/*
 * PushValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRPUSHVALACTION_HPP_
#define KEYWORDS_ARRPUSHVALACTION_HPP_

#include "../Keyword.hpp"
class ArrPushValAction: public OpKeyword {
public:
	ArrPushValAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arrPush";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    std::vector<SubStrSV> params = Object(runtime.memMan, "","",exp.data).params;
	    if(params.size() < 2)
	    	throw std::string("arrPush(<object/array>,<value as Ascal expression>)");
	    double valueToPush = runtime.callOnFrame(frame,params[1].data);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *list = runtime.resolveNextExprSafe(frame, vns);
	    list->pushList(valueToPush);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"pushed value "<<params[1].data<<" to list "<<params[0].data<<" at position: "<<(list->getListSize())<<"\n";
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_PUSHVALACTION_HPP_ */
