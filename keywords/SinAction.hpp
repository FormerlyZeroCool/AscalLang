/*
 * SinAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SINACTION_HPP_
#define KEYWORDS_SINACTION_HPP_

#include "../Keyword.hpp"
static inline void sinAction(KeywordExecutionContext ctx) 
{
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(sin(input));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"sin("<<input<<") = "<<sin(input)<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class SinAction: public OpKeyword {
public:
	SinAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "sin";
		this->operation = sinAction;
	}
};

#endif /* KEYWORDS_SINACTION_HPP_ */
