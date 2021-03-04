/*
 * TanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TANACTION_HPP_
#define KEYWORDS_TANACTION_HPP_

#include "../Keyword.hpp"
class TanAction: public OpKeyword {
public:
	TanAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "tan";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(tan(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"tan("<<input<<") = "<<tan(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_TANACTION_HPP_ */
