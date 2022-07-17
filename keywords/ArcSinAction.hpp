/*
 * ArcSin.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCSIN_HPP_
#define KEYWORDS_ARCSIN_HPP_

#include "../Keyword.hpp"
static inline void arcSinAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand &input = ctx.frame().initialOperands.back();
    input.number() = (asin(input.number()));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"arcSin("<<input.number()<<") = "<<asin(input.number())<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class ArcSinAction: public OpKeyword {
public:
	ArcSinAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arcsin";
		this->operation = arcSinAction;
	}
};

#endif /* KEYWORDS_ARCSIN_HPP_ */
