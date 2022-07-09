/*
 * PrintBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTBOOLSETTING_HPP_
#define KEYWORDS_PRINTBOOLSETTING_HPP_

#include "../Keyword.hpp"
class PrintBoolSetting: public StKeyword {
public:
	PrintBoolSetting(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "p";
	}
	void action(AscalFrame<double>* frame) override
	{
	    runtime.updateBoolSetting(frame);
	}
};

#endif /* KEYWORDS_PRINTBOOLSETTING_HPP_ */
