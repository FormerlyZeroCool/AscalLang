/*
 * FlushOutAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_FLUSHOUTACTION_HPP_
#define KEYWORDS_FLUSHOUTACTION_HPP_

#include "../Keyword.hpp"
class FlushOutAction: public StKeyword {
public:
	FlushOutAction(AscalExecutor &runtime):
		StKeyword(runtime)
	{
		this->keyWord = "flush";
	}
	void action(AscalFrame<double>* frame) override
	{
	    std::fflush(stdout);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"Flushing std out"<<'\n';
	    }
	}
};

#endif /* KEYWORDS_FLUSHOUTACTION_HPP_ */
