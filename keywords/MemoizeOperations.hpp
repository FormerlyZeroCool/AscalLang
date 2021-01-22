/*
 * MemoizeOperations.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_MEMOIZEOPERATIONS_HPP_
#define KEYWORDS_MEMOIZEOPERATIONS_HPP_

#include "../Keyword.hpp"
class MemoizeOperations: public Keyword {
public:
	MemoizeOperations(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "memoize";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    runtime->updateBoolSetting(frame);
	    return MAX;
	}
};

#endif /* KEYWORDS_MEMOIZEOPERATIONS_HPP_ */
