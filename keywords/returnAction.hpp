/*
 * returnAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_RETURNACTION_HPP_
#define KEYWORDS_RETURNACTION_HPP_
#include "../Keyword.hpp"
class returnAction: public Keyword {
public:
	returnAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "return";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		frame->index += keyWord.length();
		uint32_t index = frame->index;
	    SubStr objname = ParsingUtil::getVarName(frame->exp, frame->index);
	    Object *obj;
	    if(runtime->getCachedRtnObject())
	    {
	    	frame->returnPointer = runtime->getCachedRtnObject();
	    	runtime->setCachedRtnObject(nullptr);
	    }
	    if(frame->returnPointer)
	    {
	    	uint32_t endIndex = frame->index;
	    	while(frame->exp[endIndex] && frame->exp[++endIndex] != ';') {}
	    	string_view rtnVal(frame->exp, index, endIndex);
	    	if(ParsingUtil::isObj(rtnVal))
	    	{
	    	    obj = runtime->resolveNextExprSafe(frame, objname);
	    		frame->returnPointer->setObjectReturnRegister(obj);
		        if(*(*boolsettings)["o"])
		        {
		            std::cout<<"Returning "<<(obj?"object":"value of expression")<<": "<<(obj?obj->toString():rtnVal.str())<<'\n';
		        }
	    	}
	    	else
	    	{
	    		frame->returnPointer->setReturnFlag(true);
	    		frame->returnPointer->initialOperands.push_back(runtime->callOnFrame(frame, rtnVal.str()));
	    	}
	    }
	    while(!frame->initialOperands.isEmpty())
	    	frame->initialOperands.pop();
	    while(!frame->initialOperators.isEmpty())
	    	frame->initialOperators.pop();
	    return "a";
	}
};

#endif /* KEYWORDS_RETURNACTION_HPP_ */
