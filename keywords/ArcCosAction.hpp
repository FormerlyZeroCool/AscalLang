/*
 * ArcCosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCCOSACTION_HPP_
#define KEYWORDS_ARCCOSACTION_HPP_

#include "../Keyword.hpp"
class ArcCosAction: public OpKeyword {
public:
	ArcCosAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arccos";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(acos(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"arccos("<<input<<") = "<<acos(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};


#endif /* KEYWORDS_ARCCOSACTION_HPP_ */
