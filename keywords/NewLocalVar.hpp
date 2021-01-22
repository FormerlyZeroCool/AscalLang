/*
 * NewLocalVar.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWLOCALVAR_HPP_
#define KEYWORDS_NEWLOCALVAR_HPP_

#include "../Keyword.hpp"
class NewLocalVar: public Keyword {
public:
	NewLocalVar(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "loc";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr localName = ParsingUtil::getVarName(frame->exp,frame->exp.find("loc",frame->index)+4);
	    static uint32_t startOfExp;
	    startOfExp = frame->exp.find('=')+1;
	    SubStr subexp = ParsingUtil::getExpr(frame->exp, startOfExp, runtime->ascal_cin);
	    if(!ParsingUtil::isObj(subexp.data))
	    {
	    	Object newLocal = Object(localName.data,subexp.data,"");
	    	(*frame->getLocalMemory())[newLocal.id] = newLocal;
	        if(*(*boolsettings)["o"])
	        {
	            std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<newLocal.instructionsToFormattedString()<<std::endl;
	        }
	    }
	    else
	    {
	    	SubStr rightVarStart = ParsingUtil::getVarName(frame->exp,startOfExp);
	    	Object *newVar = runtime->resolveNextExprSafe(frame, rightVarStart);
	    	(*frame->getLocalMemory())[newVar->id] = *newVar;
	        if(*(*boolsettings)["o"])
	        {
	            std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<newVar->toString()<<std::endl;
	        }
	    }
	    return MAX;
	}
};

#endif /* KEYWORDS_NEWLOCALVAR_HPP_ */
