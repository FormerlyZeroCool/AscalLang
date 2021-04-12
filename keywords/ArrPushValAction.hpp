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
private:
    ParsedStatementList params;
public:
	ArrPushValAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arrPush";
	}
	void action(AscalFrame<double>* frame) override
	{
        params.statements.clear();
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
        params = ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 2)
	    	throw std::string("arrPush(<object/array>,<value as Ascal expression>)");
	    double valueToPush = runtime.callOnFrame(frame,params.statements[1].data);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
	    Object *list = runtime.resolveNextExprSafe(frame, vns);
	    list->pushList(valueToPush);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"pushed value "<<params.statements[1].data<<" to list "<<params.statements[0].data<<" at position: "<<(list->getListSize())<<"\n";
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_PUSHVALACTION_HPP_ */
