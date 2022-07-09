/*
 * PrintBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTBOOLSETTING_HPP_
#define KEYWORDS_PRINTBOOLSETTING_HPP_

#include "../Keyword.hpp"
	static inline void printBoolSetting(KeywordExecutionContext ctx) 
	{
	    ctx.runtime().updateBoolSetting("p");
		ctx.frame().index += Keyword::opcodeSize();
	}
class PrintBoolSetting: public StKeyword {
public:
	PrintBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "p";
		this->operation = printBoolSetting;
	}
};

#endif /* KEYWORDS_PRINTBOOLSETTING_HPP_ */
