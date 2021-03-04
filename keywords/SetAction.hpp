/*
 * SetAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SETACTION_HPP_
#define KEYWORDS_SETACTION_HPP_

#include "../Keyword.hpp"
class SetAction: public StKeyword {
public:
	SetAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "set";
	}
	void action(AscalFrame<double>* frame) override
	{

		static const std::string keyWord = "set";
		uint32_t index = frame->exp.find(keyWord,frame->index)+keyWord.size();
	    SubStrSV varName = ParsingUtil::getVarNameSV(frame->exp, index);
	    uint32_t startIndex = frame->exp.find("=",varName.end)+1;
	    while(frame->exp[startIndex] && frame->exp[startIndex] == ' ')
	        startIndex++;
	    string_view subexp;
	    {
	    	uint32_t sibk = startIndex;
	    	subexp = ParsingUtil::getExprInStringSV(frame->exp,startIndex, '{', '}', ';');
	    	startIndex = sibk;
	    }
	    Object *obj = runtime.resolveNextExprSafe(frame, varName);

	    if(!ParsingUtil::isObj(subexp))
	    {
	        //*obj = Object(runtime.memMan, obj->id,"      ","");
	    	if(!ParsingUtil::isDouble(subexp))
	    	{
	    		obj->setDouble(runtime.callOnFrame(frame,subexp));
	    	}
	    	else
	    	{
	    		char tmp = subexp[subexp.size()];
	    		subexp[subexp.size()] = 0;
	    		obj->setDouble(atof(&subexp[0]));
	    		subexp[subexp.size()] = tmp;
	    	}
    	    if(*runtime.boolsettings["o"])
    	    {
    	    	std::cout<<"set "<<varName.data<<" = "<< obj->getDouble()<<"\n";
    	    }
	    }
	    else
	    {
	    	SubStrSV rightHandObjectLookup = ParsingUtil::getVarNameSV(frame->exp, startIndex);
	    	Object *rightObj = runtime.resolveNextExprSafe(frame, rightHandObjectLookup);
	    	*obj = *rightObj;
		    if(*runtime.boolsettings["o"])
		    {
		    	std::cout<<"set "<<varName.data<<" = "<<obj->toString()<<"\n";
		    }
	    }
	}
};

#endif /* KEYWORDS_SETACTION_HPP_ */
