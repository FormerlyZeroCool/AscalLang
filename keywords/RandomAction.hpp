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

	static inline void randomAction(KeywordExecutionContext ctx) 
	{
		double hash = PRNG::ascalPRNG();
		ctx.frame().initialOperands.push(hash);
		#ifdef debug
	    if(*ctx.runtime().boolsettings["o"])
	    {
	    	std::cout<<"rand = "<<ParsingUtil::to_string(hash)<<"\n";
	    }
		#endif
	    ctx.frame().index += Keyword::opcodeSize();
	}
class RandomAction: public OpKeyword {
public:
	RandomAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "rand";
		this->operation = randomAction;
	}
};

#endif /* KEYWORDS_RANDOMACTION_HPP_ */
