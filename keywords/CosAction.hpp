/*
 * CosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_COSACTION_HPP_
#define KEYWORDS_COSACTION_HPP_

#include "../Keyword.hpp"
class CosAction: public Keyword {
public:
	CosAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "cos";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(cos(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"cos("<<input<<") = "<<cos(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_COSACTION_HPP_ */
