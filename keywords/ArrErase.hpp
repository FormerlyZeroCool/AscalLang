/*
 * PushValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRERASEVALACTION_HPP_
#define KEYWORDS_ARRERASEVALACTION_HPP_

#include "../Keyword.hpp"
class ArrEraseValAction: public OpKeyword {
private:
    ParsedStatementList params;
public:
	ArrEraseValAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arrErase";
	}
	/*void action(AscalFrame<double>* frame) override
	{
        params.statements.clear();
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
        params = ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 2)
	    	throw std::string("arrErase(<object/array>,<index to be deleted as Ascal expression>)");
	    double indexToDelete = runtime.callOnFrame(frame,params.statements[1].data);
        uint32_t index = frame->index+keyWord.size()+params.statements[0].start;
	    SubStrSV vns = ParsingUtil::getVarNameSV(frame->exp, index);
	    Object *list = runtime.resolveNextExprSafe(frame, vns);
	    list->eraseList(indexToDelete);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"pushed value "<<params.statements[1].data<<" to list "<<params.statements[0].data<<" at position: "<<(list->getListSize())<<"\n";
	    }
	    frame->index = exp.end;
	}*/
};

#endif /* KEYWORDS_PUSHVALACTION_HPP_ */
