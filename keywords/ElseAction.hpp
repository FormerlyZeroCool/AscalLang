/*
 * ElseAction.hpp
 *
 *  Created on: Jan 17, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ELSEACTION_HPP_
#define KEYWORDS_ELSEACTION_HPP_

#include "../Keyword.hpp"
class ElseAction: public StKeyword {
public:
	ElseAction(AscalExecutor &runtime):
		StKeyword(runtime)
	{
		this->keyWord = "else";
	}
	void action(AscalFrame<double>* frame) override
	{
	    int index = frame->exp.find("else",frame->index) + 4;
	    while(frame->exp[index] == ' ')
	        index++;
	    SubStr codeBlock = ParsingUtil::getCodeBlock(frame->exp, index, runtime.ascal_cin);
	    if(frame->ifFlag() && !frame->ifResultFlag())
	    {
	        //if there is an if after the else this must be set
	        if(frame->exp[index] == 'i' && frame->exp[index+1] == 'f')
	        {
	    	    if(*runtime.boolsettings["o"])
	            {
	                std::cout<<"Executing else case:\n";
	            }
	            frame->setComingfromElse(true);
	        }
	        else
	        {
	            frame->setComingfromElse(false);
	            if(index + codeBlock.end < frame->exp.size())
	                index += codeBlock.end;
	            else
	                index = frame->exp.size();

	    	    if(*runtime.boolsettings["o"])
	            {
	                std::cout<<"Executing else case:\n";
	            }
	            try{
	                runtime.callOnFrame(frame,codeBlock.data);
	            }
	            catch(std::string &exception)
	            {
	                throw std::string(exception + "\nIn else body subexp: ");
	            }

	        }
	    }
	    else
	    {
		    if(*runtime.boolsettings["o"])
	        {
	            std::cout<<"Skipping else case\n";
	        }
	        if(index + codeBlock.end - 1 < frame->exp.size())
	            index += codeBlock.end-1;
	        else
	        	index = frame->exp.size();
	    }
	    frame->setIfFlag(false);
	    frame->index = index;
	}
};

#endif /* KEYWORDS_ELSEACTION_HPP_ */
