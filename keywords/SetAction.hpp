/*
 * SetAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SETACTION_HPP_
#define KEYWORDS_SETACTION_HPP_

#include "../Keyword.hpp"
class SetAction: public Keyword {
public:
	SetAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "set";
	}
	std::string action(AscalFrame<double>* frame) override
	{

		static const std::string keyWord = "set";
	    SubStr varName = ParsingUtil::getVarName(frame->exp,frame->exp.find(keyWord,frame->index)+keyWord.size());
	    int startIndex = frame->exp.find("=",varName.end)+1;
	    while(frame->exp[startIndex] && frame->exp[startIndex] == ' ')
	        startIndex++;
	    SubStr subexp = ParsingUtil::getExpr(frame->exp,startIndex,runtime->ascal_cin);
	    std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame,subexp.data));
	    Object *obj = runtime->resolveNextExprSafe(frame, varName);
	    if(!ParsingUtil::isObj(subexp.data))
	    {
	        std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame,subexp.data));
	        *obj = Object(obj->id,value,"");
	    }
	    else
	    {
	    	SubStr rightHandObjectLookup = ParsingUtil::getVarName(frame->exp, startIndex);
	    	Object *rightObj = runtime->resolveNextExprSafe(frame, rightHandObjectLookup);
	    	*obj = *rightObj;
	    }

	    return MAX;
	}
};

#endif /* KEYWORDS_SETACTION_HPP_ */
