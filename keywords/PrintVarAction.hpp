/*
 * PrintVarAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTVARACTION_HPP_
#define KEYWORDS_PRINTVARACTION_HPP_

#include "../Keyword.hpp"
class PrintVarAction: public StKeyword {
public:
	PrintVarAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "print var";
	}
	void action(AscalFrame<double>* frame) override
	{
		std::string data;
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
		SubStr varName = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size());
	    std::cout<<"print var "<<exp.data.substr(0,exp.data.find(';'))<<" = "<<runtime.resolveNextExprSafe(frame, varName)->toString()<<"\n";
	}
};

#endif /* KEYWORDS_PRINTVARACTION_HPP_ */
