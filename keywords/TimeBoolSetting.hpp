/*
 * TimeBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TIMEBOOLSETTING_HPP_
#define KEYWORDS_TIMEBOOLSETTING_HPP_

#include "../Keyword.hpp"
class TimeBoolSetting: public Keyword {
public:
	TimeBoolSetting(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "t";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    runtime->updateBoolSetting(frame);
	    return MAX;
	}
};

#endif /* KEYWORDS_TIMEBOOLSETTING_HPP_ */
