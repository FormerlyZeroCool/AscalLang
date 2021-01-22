/*
 * SinAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SINACTION_HPP_
#define KEYWORDS_SINACTION_HPP_

#include "../Keyword.hpp"
class SinAction: public Keyword {
public:
	SinAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "sin";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(sin(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"sin("<<input<<") = "<<sin(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_SINACTION_HPP_ */
