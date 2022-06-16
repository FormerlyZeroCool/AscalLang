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
class SRandomAction: public OpKeyword {
public:
	SRandomAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "srand";
	}
	void action(AscalFrame<double>* frame) override
	{
	    /*SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    double input = runtime.callOnFrame(frame,exp.data);
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
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"srand("<<(input/100000)<<") = "<<ParsingUtil::to_string(hash)<<'\n';
	    }
        frame->index = exp.end;*/
	}
};

#endif /* KEYWORDS_SRANDOMACTION_HPP_ */
