/*
 * ArcCosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCCOSACTION_HPP_
#define KEYWORDS_ARCCOSACTION_HPP_

#include "../Keyword.hpp"
static inline void arcCosAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand &input = ctx.frame().initialOperands.back();
    input.number() = (acos(input.number()));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"arccos("<<input.number()<<") = "<<acos(input.number())<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class ArcCosAction: public OpKeyword {
public:
	ArcCosAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arccos";
		this->operation = arcCosAction;
	}
};


#endif /* KEYWORDS_ARCCOSACTION_HPP_ */
