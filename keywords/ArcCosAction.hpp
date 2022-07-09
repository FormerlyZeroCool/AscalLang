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
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(acos(input));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"arccos("<<input<<") = "<<acos(input)<<'\n';
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
