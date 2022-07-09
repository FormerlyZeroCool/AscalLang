/*
 * SinAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SINACTION_HPP_
#define KEYWORDS_SINACTION_HPP_

#include "../Keyword.hpp"
class SinAction: public OpKeyword {
public:
	SinAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "sin";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExprSV(frame->exp, frame->index, keyWord, '(', ')');
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(sin(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"sin("<<input<<") = "<<sin(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_SINACTION_HPP_ */
