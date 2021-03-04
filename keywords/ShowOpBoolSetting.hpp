/*
 * ShowOpBoolSetting.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SHOWOPBOOLSETTING_HPP_
#define KEYWORDS_SHOWOPBOOLSETTING_HPP_

#include "../Keyword.hpp"
class ShowOpBoolSetting: public StKeyword {
public:
	ShowOpBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "o";
	}
	void action(AscalFrame<double>* frame) override
	{
	    runtime.updateBoolSetting(frame);
	}
};

#endif /* KEYWORDS_SHOWOPBOOLSETTING_HPP_ */
