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
class RandomAction: public Keyword {
public:
	RandomAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "rand";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		double hash = PRNG::ascalPRNG();
		frame->initialOperands.push(hash);
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"rand = "<<ParsingUtil::to_string(hash)<<"\n";
	    }
	    return 'a'+frame->exp.substr(frame->index+4,frame->exp.size());
	}
};

#endif /* KEYWORDS_RANDOMACTION_HPP_ */
