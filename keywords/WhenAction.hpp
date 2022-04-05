/*
 * WhenAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHENACTION_HPP_
#define KEYWORDS_WHENACTION_HPP_

#include "../Keyword.hpp"
class WhenAction: public OpKeyword {
	private:
	const uint32_t bufSize = 8192;
	char buffer[8192];
    std::string thenText, elseText;
public:
	WhenAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "when";
        this->thenText = "then";
        this->elseText = "else";
	}
	void action(AscalFrame<double>* frame) override
	{
	    //parse expression, start from substr when go up until substr end is found
	    //extract substring, and save the other parts of the expression
	    //find when evaluate expression between it and then
	    //if the expression evaluates to anything other than 0
	    //then extract the expression proceeding the then statement
	    //frame->index = 0;
		static const int invalidIndex = 2000000000;
	    const int startIndex = frame->index;

        const string_view exp = string_view(frame->exp);
	    int endIndex = frame->index;
	    /*this block locates the end keyword for the ascal when block*/{
	    int i = endIndex;
	    uint8_t qCount = 0;
	    while(i < frame->exp.length()-2)
	    {
	    	qCount = ((frame->exp[i] == '\"') + qCount) & 1;
	    	if(frame->exp[i] == 'e' && frame->exp[i+1] == 'n' && frame->exp[i+2] == 'd' && qCount == 0)
	    	{
	    		endIndex = i;
	    		i = invalidIndex;
	    	}
	    	i++;
	    }
	    }
	    //string_view endOfExp = string_view(frame->exp).substr(endIndex+3,frame->exp.length());
	    string_view value;
	    int index = startIndex + 4;
	    //should always start after when is finished to build boolean expression

        //find boolean expression after when
        //find code block to exec if be evals to true
        //find out if next is when, else, or end
        //if when take when to end extract to function frame with parent of current push to stack set index to after end
        //if else take code block after else and put on stack set index to after end
        //if end set index to after end
        //then push code block to be exec if true on stack
        //then push conditional onto stack with boolean expression
	    int thenIndex;
	    int whenIndex = startIndex;
	    double boolExpValue;
	    int elseIndex = frame->exp.find(this->elseText,frame->index);
	    elseIndex = elseIndex==-1?invalidIndex:elseIndex;
        
            thenIndex = frame->exp.find(this->thenText,index);

            //Parsing boolean expression
            while(frame->exp[index] == ' ' && index < endIndex && index < thenIndex)
            {
                index++;
            }
            uint32_t startOfBoolExp = index;
            while(frame->exp.size() > index && index < endIndex && index < thenIndex)
            {
                index++;
            }
            string_view booleanExpression = exp.substr(startOfBoolExp,index-startOfBoolExp);
        
	        if(booleanExpression.size() == 0)
	        {
	            throw std::string("Error no boolean expression provided in when.\n");
	        }

		    if(*runtime.boolsettings["o"])
	        {
	            std::cout<<"Executing Boolean Expression: "<<booleanExpression<<'\n';
	        }
            
            index += 5;//increment index past this when
            thenIndex = frame->exp.find(this->keyWord,index);//find next when
            thenIndex = thenIndex==-1?endIndex+1:thenIndex;//find end token of code block
            const int codeBlockEndIndex = std::min(std::min(endIndex,thenIndex),elseIndex);
            value = ParsingUtil::getExprInStringSV(exp.substr(index, codeBlockEndIndex - index), 0, '{', '}',';').data;//get code block
        if(codeBlockEndIndex == elseIndex)
        {
            frame->index = codeBlockEndIndex + 4;
            string_view remainder = frame->exp.substr(codeBlockEndIndex + 4, endIndex - (codeBlockEndIndex + 4));
                
            AscalFrame<double> *executionFrame = runtime.sFramePool.construct(runtime, frame);
            executionFrame->returnPointer = frame;
            executionFrame->exp = remainder;
            executionFrame->setIsDynamicAllocation(true);
            executionFrame->setContext(frame->getContext());
            runtime.currentStack->push(executionFrame);
        }
        else if(codeBlockEndIndex == endIndex+1)
        {
                
            AscalFrame<double> *executionFrame = runtime.sFramePool.construct(runtime, frame);
            executionFrame->returnPointer = frame;
            executionFrame->exp = "";
            executionFrame->setIsDynamicAllocation(true);
            executionFrame->setContext(frame->getContext());
            runtime.currentStack->push(executionFrame);
        }
        else
        {
            string_view remainder = frame->exp.substr(codeBlockEndIndex, 3 + endIndex - codeBlockEndIndex);
                
            AscalFrame<double> *executionFrame = runtime.sFramePool.construct(runtime, frame);
            executionFrame->returnPointer = frame;
            executionFrame->exp = remainder;
            executionFrame->setIsDynamicAllocation(true);
            executionFrame->setContext(frame->getContext());
            runtime.currentStack->push(executionFrame);
        }
                                                   
        {
            AscalFrame<double> *executionFrame = runtime.sFramePool.construct(runtime, frame);
            executionFrame->returnPointer = frame;
            executionFrame->exp = value;
            executionFrame->setIsDynamicAllocation(true);
            executionFrame->setContext(frame->getContext());
            runtime.currentStack->push(executionFrame);
        }
        {
            AscalFrame<double> *executionFrame = runtime.cFramePool.construct(runtime, frame);
            executionFrame->returnPointer = frame;
            executionFrame->exp = booleanExpression;
            executionFrame->setIsDynamicAllocation(true);
            executionFrame->setContext(frame->getContext());
            runtime.currentStack->push(executionFrame);
        }
	    if(*runtime.boolsettings["o"])
	    {
	        std::cout<<"Executing Branch: "<<value<<" Params: "<<AscalExecutor::printMemory(*frame->getParamMemory()," = ",false,"|");
	        std::cout<<"\n";
	    }
        //frame->initialOperands.push(runtime.callOnFrame(frame, value));
        frame->index = endIndex+3;
	}
};

#endif /* KEYWORDS_WHENACTION_HPP_ */
