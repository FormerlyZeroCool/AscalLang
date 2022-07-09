/*
 * MemoizeOperations.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_MEMOIZEOPERATIONS_HPP_
#define KEYWORDS_MEMOIZEOPERATIONS_HPP_

#include "../Keyword.hpp"
	static inline void memoizeOperations(KeywordExecutionContext ctx) 
	{
	    ctx.runtime().updateBoolSetting("memoize");
		ctx.frame().index += Keyword::opcodeSize();
	}
class MemoizeOperations: public StKeyword {
public:
	MemoizeOperations(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "memoize";
		this->operation = memoizeOperations;
	}
};

#endif /* KEYWORDS_MEMOIZEOPERATIONS_HPP_ */
