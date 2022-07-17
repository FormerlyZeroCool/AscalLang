/*
 * CosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_COSACTION_HPP_
#define KEYWORDS_COSACTION_HPP_

#include "../Keyword.hpp"
static inline void cosAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand &input = ctx.frame().initialOperands.back();
    input.number() = (input.number());
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"cos("<<input.number()<<") = "<<cos(input.number())<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class CosAction: public OpKeyword {
public:
	CosAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "cos";
		this->operation = cosAction;
	}
};

#endif /* KEYWORDS_COSACTION_HPP_ */
