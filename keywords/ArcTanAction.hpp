/*
 * ArcTanAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCTANACTION_HPP_
#define KEYWORDS_ARCTANACTION_HPP_

#include "../Keyword.hpp"
class ArcTanAction: public Keyword {
public:
	ArcTanAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arctan";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(atan(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"arctan("<<input<<") = "<<atan(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};


#endif /* KEYWORDS_ARCTANACTION_HPP_ */
