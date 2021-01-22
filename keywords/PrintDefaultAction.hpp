/*
 * PrintDefaultAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTDEFAULTACTION_HPP_
#define KEYWORDS_PRINTDEFAULTACTION_HPP_

#include "../Keyword.hpp"
#include "PrintStringAction.hpp"
#include "PrintCommand.hpp"
class PrintDefaultAction: public Keyword {
private:
	PrintStringAction ps;
	PrintCommand pc;
public:
	PrintDefaultAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings), ps(runtime, memory, boolsettings), pc(runtime, memory, boolsettings)
	{
		this->keyWord = "print";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    int endOfStatement = frame->index;
	    while(frame->exp[endOfStatement] && (frame->exp[endOfStatement] != ';' && frame->exp[endOfStatement] != '\n' &&
	                                         frame->exp[endOfStatement] != '\0'))
	    {
	        endOfStatement++;
	    }
	    uint32_t quoteIndex = frame->exp.find("\"",frame->index);
	    if(quoteIndex != -1 && quoteIndex < endOfStatement)
	    {
	        ps.action(frame);
	    }
	    else
	    {
	        pc.action(frame);
	    }
	    return MAX;
	}

};

#endif /* KEYWORDS_PRINTDEFAULTACTION_HPP_ */
