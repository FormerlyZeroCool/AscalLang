/*
 * SleepAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SLEEPACTION_HPP_
#define KEYWORDS_SLEEPACTION_HPP_

#include "../Keyword.hpp"
static inline void sleepAction(KeywordExecutionContext ctx) 
{
	double input = 0;
	ctx.frame().initialOperands.top(input);
	ctx.frame().initialOperands.pop();
    CrossPlatform::usleep(input * 100);
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"sleep("<<input<<") \n";
    }
	#endif
    ctx.frame().index += Keyword::opcodeSize();
}
class SleepAction: public OpKeyword {
public:
	SleepAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "sleep";
		this->operation = sleepAction;
	}
};

#endif /* KEYWORDS_SLEEPACTION_HPP_ */
