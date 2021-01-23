/*
 * ArcCosAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCCOSACTION_HPP_
#define KEYWORDS_ARCCOSACTION_HPP_

#include "../Keyword.hpp"
class ArcCosAction: public Keyword {
public:
	ArcCosAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arccos";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(acos(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"arccos("<<input<<") = "<<acos(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};


#endif /* KEYWORDS_ARCCOSACTION_HPP_ */
