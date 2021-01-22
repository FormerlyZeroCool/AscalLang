/*
 * PrintVarAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTVARACTION_HPP_
#define KEYWORDS_PRINTVARACTION_HPP_

#include "../Keyword.hpp"
class PrintVarAction: public Keyword {
public:
	PrintVarAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "print var";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		std::string data;
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
		SubStr varName = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size());
	    std::cout<<"print var "<<exp.data.substr(0,exp.data.find(';'))<<" = "<<runtime->resolveNextExprSafe(frame, varName)->toString()<<"\n";
	    return MAX;
	}
};

#endif /* KEYWORDS_PRINTVARACTION_HPP_ */
