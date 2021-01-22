/*
 * PrintBoolSetting.hpp
 *
 *  Created on: Jan 19, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTBOOLSETTING_HPP_
#define KEYWORDS_PRINTBOOLSETTING_HPP_

#include "../Keyword.hpp"
class PrintBoolSetting: public Keyword {
public:
	PrintBoolSetting(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "p";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    runtime->updateBoolSetting(frame);
	    return MAX;
	}
};

#endif /* KEYWORDS_PRINTBOOLSETTING_HPP_ */
