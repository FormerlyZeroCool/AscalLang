/*
 * returnAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_RETURNACTION_HPP_
#define KEYWORDS_RETURNACTION_HPP_
#include "../Keyword.hpp"
class returnAction: public OpKeyword {
public:
	returnAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "return";
	}
	void action(AscalFrame<double>* frame) override
	{
		frame->index += keyWord.length();
		uint32_t index = frame->index;
	    SubStr objname = ParsingUtil::getVarName(frame->exp, frame->index);
	    Object *obj;
	    if(runtime.getCachedRtnObject())
	    {
	    	frame->returnPointer = runtime.getCachedRtnObject();
	    	runtime.setCachedRtnObject(nullptr);
	    }
	    if(frame->returnPointer)
	    {
	    	uint32_t endIndex = frame->index;
	    	while(frame->exp[endIndex] && frame->exp[++endIndex] != ';') {}
	    	string_view rtnVal(frame->exp, index, endIndex);
	    	if(ParsingUtil::isObj(rtnVal))
	    	{
	    	    obj = runtime.resolveNextExprSafe(frame, objname);
	    		frame->returnPointer->setObjectReturnRegister(obj);
	    	    if(*runtime.boolsettings["o"])
		        {
		            std::cout<<"Returning "<<(obj?"object":"value of expression")<<": "<<(obj?obj->toString():rtnVal.str())<<'\n';
		        }
	    	}
	    	else
	    	{
	    		frame->returnPointer->setReturnFlag(true);
	    		frame->returnPointer->initialOperands.push_back(runtime.callOnFrame(frame, rtnVal.str()));
	    	}
	    }
	    while(!frame->initialOperands.isEmpty())
	    	frame->initialOperands.pop();
	    frame->index = frame->exp.size();
	}
};

#endif /* KEYWORDS_RETURNACTION_HPP_ */
