/*
 * ArcTanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCTANACTION_HPP_
#define KEYWORDS_ARCTANACTION_HPP_

#include "../Keyword.hpp"
static inline void arcTanAction(KeywordExecutionContext ctx) 
{
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(atan(input));
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"arctan("<<input<<") = "<<atan(input)<<'\n';
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class ArcTanAction: public OpKeyword {
public:
	ArcTanAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arctan";
		this->operation = arcTanAction;
	}
};


#endif /* KEYWORDS_ARCTANACTION_HPP_ */
