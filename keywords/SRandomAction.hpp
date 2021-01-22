/*
 * SRandomAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SRANDOMACTION_HPP_
#define KEYWORDS_SRANDOMACTION_HPP_

#include "../Keyword.hpp"
#include "../PRNG.hpp"
class SRandomAction: public Keyword {
public:
	SRandomAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "srand";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    double input = runtime->callOnFrame(frame,exp.data);
	    double ib = input/100000;
	    input *= 100000;
	    input += ib;
	    if(exp.data.length() > 5)
	    {
	    	PRNG::hr2 = input;
	    	PRNG::hashRand = input;
	    }
	    else
	    {
	    	//can also use time(NULL) instead of clock()
	    	input = PRNG::hashRand + clock();
	    	PRNG::hr2 = input;
	    	PRNG::hashRand = input;
	    }
	    double hash = PRNG::ascalPRNG();
		frame->initialOperands.push(hash);
        if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"srand("<<(input/100000)<<") = "<<ParsingUtil::to_string(hash)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_SRANDOMACTION_HPP_ */
