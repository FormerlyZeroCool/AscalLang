/*
 * ArcSin.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARCSIN_HPP_
#define KEYWORDS_ARCSIN_HPP_

#include "../Keyword.hpp"
class ArcSinAction: public Keyword {
public:
	ArcSinAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "arcsin";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    frame->initialOperands.push(asin(input));
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"arcsin("<<input<<") = "<<asin(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_ARCSIN_HPP_ */
