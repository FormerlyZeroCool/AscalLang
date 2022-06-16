/*
 * TimeBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TIMEBOOLSETTING_HPP_
#define KEYWORDS_TIMEBOOLSETTING_HPP_

#include "../Keyword.hpp"
	static inline void timeBoolSetting(KeywordExecutionContext ctx) 
	{
	    ctx.runtime().updateBoolSetting("t");
		ctx.frame().index += Keyword::opcodeSize();
	}
class TimeBoolSetting: public StKeyword {
public:
	TimeBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "t";
		this->operation = timeBoolSetting;
	}
};

#endif /* KEYWORDS_TIMEBOOLSETTING_HPP_ */
