/*
 * SimplifyFunctionAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SIMPLIFYFUNCTIONACTION_HPP_
#define KEYWORDS_SIMPLIFYFUNCTIONACTION_HPP_

#include "../Keyword.hpp"
class SimplifyFunctionAction: public StKeyword {
public:
	SimplifyFunctionAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "simplify";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    if(exp.data.size() == 1)
	    	throw std::string("simplify <function>");
	    //Get definition by calling function.getIntsructions(), or for formatted version getInstructionsFormated
	    Object* function;
	    try{
	        SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size());
	    	function = runtime.resolveNextExprSafe(frame, vns);
	    }catch(std::string &objNotFoundErr)
	    {
	    	throw std::string("Error could not find function: " + exp.data + " to simplify.");
	    }
	    //your code

	    //end of your code is defining simplifiedfn
	    std::string simplifiedfn;
	    std::stringstream call;
	    call<<"set "<<exp.data<<" = "<<simplifiedfn;
	    runtime.callOnFrame(frame, call.str());
	}
};

#endif /* KEYWORDS_SIMPLIFYFUNCTIONACTION_HPP_ */
