/*
 * EvalSetZF.hpp
 *
 *  Created on: Jan 27, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_EVALSETZF_HPP_
#define KEYWORDS_EVALSETZF_HPP_

#include "../Keyword.hpp"
#include "../Vect2D.hpp"
class EvalSetZF: public OpKeyword {
public:
	EvalSetZF(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "evalSetZF";
	}

	void action(AscalFrame<double>* frame) override
	{
		double sv = std::numeric_limits<double>::max();
		frame->initialOperands.top(sv);
		if(sv == std::numeric_limits<double>::max())
			throw std::string("Error no boolean to evaluate");
		frame->initialOperands.pop();
		frame->setZeroFlag(!sv);
		frame->index += this->keyWord.size();
	}
};

#endif /* KEYWORDS_EVALSETZF_HPP_ */
