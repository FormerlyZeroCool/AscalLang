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
class PrintDefaultAction: public StKeyword {
private:
	PrintStringAction ps;
	PrintCommand pc;
public:
	PrintDefaultAction(AscalExecutor &runtime):
	StKeyword(runtime), ps(runtime), pc(runtime)
	{
		this->keyWord = "print";
	}
	void action(AscalFrame<double>* frame) override
	{
	    int endOfStatement = frame->index;
	    while(frame->exp.size() > endOfStatement && frame->exp[endOfStatement] != ';' && frame->exp[endOfStatement] != '\n')
	    {
	        endOfStatement++;
	    }
	    uint32_t quoteIndex = frame->exp.find("\"",frame->index + this->opcodeSize());
	    if(quoteIndex != -1 && quoteIndex < endOfStatement)
	    {
	        ps.action(frame);
	    }
	    else
	    {
	        pc.action(frame);
	    }
	}

};

#endif /* KEYWORDS_PRINTDEFAULTACTION_HPP_ */
