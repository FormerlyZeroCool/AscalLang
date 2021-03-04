/*
 * JumpNZ.hpp
 *
 *  Created on: Jan 27, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_JUMPNZ_HPP_
#define KEYWORDS_JUMPNZ_HPP_

#include "../Keyword.hpp"
class JumpNZ: public OpKeyword {
public:
	JumpNZ(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "jumpNZ";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    int index = 0;
	    double input = ParsingUtil::getNextDoubleS(exp.data, index);
	    uint32_t jnzSize = this->keyWord.size()+2+(uint32_t) (std::log(input)/std::log(10.0));
	    if(!frame->isZeroFlagSet())
	    	frame->index += input+jnzSize;
	    else
	    	frame->index += jnzSize;

	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"jumpNZ("<<input<<") = "<<(frame->index)<<'\n';
	    }
	}
};

#endif /* KEYWORDS_JUMPNZ_HPP_ */
