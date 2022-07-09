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
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(cos(input));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"cos("<<input<<") = "<<cos(input)<<'\n';
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
