/*
 * SleepAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SLEEPACTION_HPP_
#define KEYWORDS_SLEEPACTION_HPP_

#include "../Keyword.hpp"
class SleepAction: public OpKeyword {
public:
	SleepAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "sleep";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame, exp.data);

	    CrossPlatform::usleep(((long)input));

	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"sleeping for "<<input<<" micro-seconds\n";
	    }
        frame->index = exp.end;
	}
};

#endif /* KEYWORDS_SLEEPACTION_HPP_ */
