/*
 * ShowOpBoolSetting.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SHOWOPBOOLSETTING_HPP_
#define KEYWORDS_SHOWOPBOOLSETTING_HPP_

#include "../Keyword.hpp"
class ShowOpBoolSetting: public Keyword {
public:
	ShowOpBoolSetting(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "o";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    runtime->updateBoolSetting(frame);
	    return MAX;
	}
};

#endif /* KEYWORDS_SHOWOPBOOLSETTING_HPP_ */
