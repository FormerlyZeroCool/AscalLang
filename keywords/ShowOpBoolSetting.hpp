/*
 * ShowOpBoolSetting.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SHOWOPBOOLSETTING_HPP_
#define KEYWORDS_SHOWOPBOOLSETTING_HPP_

#include "../Keyword.hpp"

	static inline void showOpBoolSetting(KeywordExecutionContext ctx) 
	{
	    ctx.runtime().updateBoolSetting("o");
		ctx.frame().index += Keyword::opcodeSize();
	}
class ShowOpBoolSetting: public StKeyword {
public:
	ShowOpBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "o";
		this->operation = showOpBoolSetting;
	}
};

#endif /* KEYWORDS_SHOWOPBOOLSETTING_HPP_ */
