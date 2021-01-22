/*
 * DerivefnAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_DERIVEFNACTION_HPP_
#define KEYWORDS_DERIVEFNACTION_HPP_

#include "../Keyword.hpp"
class DerivefnAction: public Keyword {
public:
	DerivefnAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "derive";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		//Feel free to change anything, I've just added the boilerplate, and some convenience code for you
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    //you can change this, just getting some params for you
	    if(params.size()<3)
	    	throw std::string("derive <function name, variable to differentiate, derivative degree>");
	    //id is the function name, getInstructions() will return a reference to the code saved for the function as a string
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *function = runtime->resolveNextExprSafe(frame, vns);
	    std::string withRespectTo = params[1].data;
	    uint16_t degree = (uint16_t) runtime->callOnFrame(frame, params[2].data);
	    //Your code here

	    //end of your code is defining the string derivative
	    std::string derivative;
	    std::stringstream call;
	    call<<"let "<<function->id<<"prime"<<degree<<" = "<<function->getInstructions();
	    //Saves function that when it is a first derivative of f looks like
	    //fprime1 = first derivative of f
	    runtime->callOnFrame(frame, call.str());
	    //Automatically moves to next expression after keyword execution because we return MAX
	    return MAX;
	}
};

#endif /* KEYWORDS_DERIVEFNACTION_HPP_ */
