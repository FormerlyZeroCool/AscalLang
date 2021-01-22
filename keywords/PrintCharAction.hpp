/*
 * PrintCharAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTCHARACTION_HPP_
#define KEYWORDS_PRINTCHARACTION_HPP_

#include "../Keyword.hpp"
class PrintCharAction: public Keyword {
public:
	PrintCharAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "printChar";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    char input = runtime->callOnFrame(frame,exp.data);
	    std::cout<<( input);
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"printChar("<<input<<") = "<<(char)(input)<<'\n';
	    }
	    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
	}
};

#endif /* KEYWORDS_PRINTCHARACTION_HPP_ */
