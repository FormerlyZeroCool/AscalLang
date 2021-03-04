/*
 * PrintCharAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTCHARACTION_HPP_
#define KEYWORDS_PRINTCHARACTION_HPP_

#include "../Keyword.hpp"
class PrintCharAction: public StKeyword {
public:
	PrintCharAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "printChar";
	}
	void action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    char input = runtime.callOnFrame(frame,exp.data);
	    std::cout<<( input);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"printChar("<<input<<") = "<<(char)(input)<<'\n';
	    }
	    frame->index = exp.end;
	}
};

#endif /* KEYWORDS_PRINTCHARACTION_HPP_ */
