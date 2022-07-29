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
	    SubStrSV exp = ParsingUtil::getFollowingExprSV(frame->exp, frame->index, keyWord);
	    try{
			/*
	    	double input = runtime.callOnFrame(frame,exp.data);
	    	frame->initialOperands.push(input);
			*/
	    }
	    catch(std::string &s)
	    {
			/*
	    	frame->initialOperands.clear();
			frame->initialOperands.push_back(std::numeric_limits<double>::max());
			Object log(runtime.memMan, string_view("errorMessageLog", 14), string_view("", 0));
			log.loadString(s);
			Object errorFlag(runtime.memMan, string_view("errorOccurred", 13), string_view("", 0));
			errorFlag.setDouble(1);
			runtime.loadUserDefinedFn(log, *frame->getLocalMemory());
			runtime.loadUserDefinedFn(errorFlag, *frame->getLocalMemory());
			*/
	    }
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"try("<<exp.data<<")\n";
	    }
        frame->index = exp.start + exp.data.size();
	}
};

#endif /* KEYWORDS_TRYACTION_HPP_ */
