/*
 * JumpZF.hpp
 *
 *  Created on: Jan 27, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_JUMPZF_HPP_
#define KEYWORDS_JUMPZF_HPP_

#include "../Keyword.hpp"
class JumpZF: public OpKeyword {
public:
	JumpZF(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "kumpZF";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    int index = 0;
	    double input = ParsingUtil::getNextDoubleS(exp.data, index);
	    uint32_t jzSize = this->keyWord.size()+2+(uint32_t) (std::log(input)/std::log(10.0));
	    if(frame->isZeroFlagSet())
	    	frame->index += input+jzSize;
	    else
	    	frame->index += jzSize;

	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"jumpZF("<<input<<") = "<<(frame->index)<<'\n';
	    }
	}

};

#endif /* KEYWORDS_JUMPZF_HPP_ */
