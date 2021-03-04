/*
 * TryAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TRYACTION_HPP_
#define KEYWORDS_TRYACTION_HPP_

#include "../Keyword.hpp"
class TryAction: public OpKeyword {
public:
	TryAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "try";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    try{
	    	double input = runtime.callOnFrame(frame,exp.data);
	    	frame->initialOperands.push(input);
	    }
	    catch(std::string &s)
	    {
	    	frame->initialOperands.push(std::floor(std::numeric_limits<double>::max()));
	    	runtime.callOnFrame(frame, "loc errorMessageLog = {"+s+"};\nloc hasErrorOccurred = 1");
	    }
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"try("<<exp.data<<")\n";
	    }
        frame->index = exp.end;
	}
};

#endif /* KEYWORDS_TRYACTION_HPP_ */
