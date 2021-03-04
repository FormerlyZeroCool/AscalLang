/*
 * RandomAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_RANDOMACTION_HPP_
#define KEYWORDS_RANDOMACTION_HPP_

#include "../Keyword.hpp"
#include "../PRNG.hpp"
class RandomAction: public OpKeyword {
public:
	RandomAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "rand";
	}
	void action(AscalFrame<double>* frame) override
	{
		double hash = PRNG::ascalPRNG();
		frame->initialOperands.push(hash);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"rand = "<<ParsingUtil::to_string(hash)<<"\n";
	    }
	    frame->index += this->keyWord.size();
	}
};

#endif /* KEYWORDS_RANDOMACTION_HPP_ */
