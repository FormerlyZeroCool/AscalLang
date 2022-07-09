/*
 * CosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_COSACTION_HPP_
#define KEYWORDS_COSACTION_HPP_

#include "../Keyword.hpp"
class CosAction: public OpKeyword {
public:
	CosAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "cos";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
	    frame->initialOperands.push(cos(input));
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"cos("<<input<<") = "<<cos(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_COSACTION_HPP_ */
