/*
 * MemoizeOperations.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_MEMOIZEOPERATIONS_HPP_
#define KEYWORDS_MEMOIZEOPERATIONS_HPP_

#include "../Keyword.hpp"
class MemoizeOperations: public StKeyword {
public:
	MemoizeOperations(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "memoize";
	}
	void action(AscalFrame<double>* frame) override
	{
	    runtime.updateBoolSetting(frame);
	}
};

#endif /* KEYWORDS_MEMOIZEOPERATIONS_HPP_ */
