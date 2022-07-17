/*
 * TanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TANACTION_HPP_
#define KEYWORDS_TANACTION_HPP_

#include "../Keyword.hpp"
static inline void tanAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand &input = ctx.frame().initialOperands.back();
    input.number() = tan(input.number());
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"tan("<<input.number()<<") = "<<tan(input.number())<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class TanAction: public OpKeyword {
public:
	TanAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "tan";
		this->operation = tanAction;
	}
};

#endif /* KEYWORDS_TANACTION_HPP_ */
