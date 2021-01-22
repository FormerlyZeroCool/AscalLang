/*
 * TryAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TRYACTION_HPP_
#define KEYWORDS_TRYACTION_HPP_

#include "../Keyword.hpp"
class TryAction: public Keyword {
public:
	TryAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "try";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    try{
	    	double input = runtime->callOnFrame(frame,exp.data);
	    	frame->initialOperands.push(input);
	    }
	    catch(std::string &s)
	    {
	    	frame->initialOperands.push(std::floor(std::numeric_limits<double>::max()));
	    	runtime->callOnFrame(frame, "loc errorMessageLog = {"+s+"};\nloc hasErrorOccurred = 1");
	    }
        if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"try("<<exp.data<<")\n";
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_TRYACTION_HPP_ */
