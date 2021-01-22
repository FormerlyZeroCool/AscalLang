/*
 * SleepAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SLEEPACTION_HPP_
#define KEYWORDS_SLEEPACTION_HPP_

#include "../Keyword.hpp"
class SleepAction: public Keyword {
public:
	SleepAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "sleep";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame, exp.data);

	    CrossPlatform::usleep(((long)input));

        if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"sleeping for "<<input<<" micro-seconds\n";
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_SLEEPACTION_HPP_ */
