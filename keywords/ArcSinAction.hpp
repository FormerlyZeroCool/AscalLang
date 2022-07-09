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
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(asin(input));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"arcSin("<<input<<") = "<<asin(input)<<'\n';
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
