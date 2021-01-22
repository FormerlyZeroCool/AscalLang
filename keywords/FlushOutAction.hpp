/*
 * FlushOutAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_FLUSHOUTACTION_HPP_
#define KEYWORDS_FLUSHOUTACTION_HPP_

#include "../Keyword.hpp"
class FlushOutAction: public Keyword {
public:
	FlushOutAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "flush";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    std::fflush(stdout);
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"Flushing std out"<<'\n';
	    }
	    return MAX;
	}
};

#endif /* KEYWORDS_FLUSHOUTACTION_HPP_ */
