/*
 * TanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_TANACTION_HPP_
#define KEYWORDS_TANACTION_HPP_

#include "../Keyword.hpp"
class TanAction: public Keyword {
public:
	TanAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "tan";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(tan(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"tan("<<input<<") = "<<tan(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_TANACTION_HPP_ */
