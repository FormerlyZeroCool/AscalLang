/*
 * ArcTanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCTANACTION_HPP_
#define KEYWORDS_ARCTANACTION_HPP_

#include "../Keyword.hpp"
class ArcTanAction: public OpKeyword {
public:
	ArcTanAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arctan";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(atan(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"arctan("<<input<<") = "<<atan(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};


#endif /* KEYWORDS_ARCTANACTION_HPP_ */
