/*
 * NewLocalVarAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWLOCALVARACTION_HPP_
#define KEYWORDS_NEWLOCALVARACTION_HPP_

#include "../Keyword.hpp"
class NewLocalVarAction: public Keyword {
public:
	NewLocalVarAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "cloc";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr localName = ParsingUtil::getVarName(frame->exp, frame->exp.find("loc", frame->index)+4);
	    SubStr subexp = ParsingUtil::getExpr(frame->exp, frame->exp.find('=',frame->index)+1, runtime->ascal_cin);
	    std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame, subexp.data));
	    Object newLocalVar(localName.data,value,"");
	    if(*(*boolsettings)["o"])
	    {
	        std::cout<<std::endl<<"New local var: "<<localName.data<< " = "<<value<<std::endl;
	    }
	    (*frame->getLocalMemory())[newLocalVar.id] = newLocalVar;
	    return MAX;
	}
};

#endif /* KEYWORDS_NEWLOCALVARACTION_HPP_ */
