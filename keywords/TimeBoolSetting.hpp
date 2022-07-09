/*
 * TimeBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TIMEBOOLSETTING_HPP_
#define KEYWORDS_TIMEBOOLSETTING_HPP_

#include "../Keyword.hpp"
class TimeBoolSetting: public StKeyword {
public:
	TimeBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "t";
	}
	void action(AscalFrame<double>* frame) override
	{
	    runtime.updateBoolSetting(frame);
	}
};

#endif /* KEYWORDS_TIMEBOOLSETTING_HPP_ */
