/*
 * ArrSetValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRSETVALACTION_HPP_
#define KEYWORDS_ARRSETVALACTION_HPP_

#include "../Keyword.hpp"
class ArrSetValAction: public OpKeyword {
private:
    ParsedStatementList params;
public:
	ArrSetValAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arrSet";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    ParsingUtil::ParseStatementList(exp.data,0,params);
	    if(params.statements.size() < 3)
	    	throw std::string("arrGet(<array>,<Index as Ascal expression>,<value as expression>)");
	    double indexToSet = runtime.callOnFrame(frame,params.statements[1].data);
	    double value = runtime.callOnFrame(frame,params.statements[2].data);
	    Object newObj(runtime.memMan, "",ParsingUtil::to_string(value),"");
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
	    runtime.resolveNextExprSafe(frame, vns)->setList(newObj, indexToSet);
	    (runtime.memory)[string_view("null")].copyToInstructions(MAX);
	    if(*runtime.boolsettings["o"])
	    {
	    	double t;
	    	frame->initialOperands.top(t);
	    	std::cout<<"got element at index: "<<params.statements[1].data<<" from list "<<params.statements[0].data<<" value: "<<(t)<<"\n";
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_ARRSETVALACTION_HPP_ */
