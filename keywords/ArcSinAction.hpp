/*
 * ArcSin.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCSIN_HPP_
#define KEYWORDS_ARCSIN_HPP_

#include "../Keyword.hpp"
class ArcSinAction: public OpKeyword {
public:
	ArcSinAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arcsin";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(asin(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"arcsin("<<input<<") = "<<asin(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_ARCSIN_HPP_ */
