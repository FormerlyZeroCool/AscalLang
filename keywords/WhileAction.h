/*
 * WhileAction.h
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHILEACTION_H_
#define KEYWORDS_WHILEACTION_H_

#include "../Keyword.hpp"
class WhileAction: public ComplexStKeyword {
public:
	WhileAction(AscalExecutor &runtime):
		ComplexStKeyword(runtime)
	{
		this->keyWord = "while";
	}
	void action(AscalFrame<double>* frame) override
	{
	    int index = frame->index+5;
	    while(frame->exp[index] == ' ')
	        index++;

	    int startOfBoolExp = index;
	    int startOfCodeBlock = index;
	    SubStr codeBlock("",0,0);

	    while(startOfCodeBlock < frame->exp.size() && frame->exp[startOfCodeBlock] != '{')
	    {
	        startOfCodeBlock++;
	    }

	    index = startOfCodeBlock;
	    const SubStrSV booleanExpression = ParsingUtil::getExprInStringSV(frame->exp, startOfBoolExp, '(', ')', '{');
	    FunctionSubFrame<double> boolExecutionFrame(runtime, frame->getParams(), frame->getParamMemory(), frame->getLocalMemory());
	    boolExecutionFrame.setIsDynamicAllocation(false);
	    boolExecutionFrame.exp = booleanExpression.data;
	    boolExecutionFrame.setContext(frame->getContext());
	    if(booleanExpression.data.size() == 0)
	    {
	        throw std::string("Error no boolean expression provided in while.\n");
	    }
	    double boolExpValue;
	    try{
		    if(*runtime.boolsettings["o"])
	        {
	            std::cout<<"Executing While Boolean Expression: "<<booleanExpression.data<<"\n";
	        }
	        boolExpValue = runtime.calculateExpression(&boolExecutionFrame);
	    }
	    catch(std::string &exception)
	    {
	        throw std::string(exception +"\nIn While Boolean Expression");
	    }
	    if(*runtime.boolsettings["o"])
	    {
	        std::cout<<"Execution Complete. "<<(boolExpValue?"true":"false")<<"\n\n";
	    }

	    codeBlock = ParsingUtil::getCodeBlock(frame->exp, index, runtime.ascal_cin);
	    FunctionSubFrame<double> executionFrame(runtime, frame->getParams(), frame->getParamMemory(), frame->getLocalMemory());
	    executionFrame.exp = codeBlock.data;
	    executionFrame.setIsDynamicAllocation(false);
	    executionFrame.setContext(frame->getContext());
	    if(boolExpValue != 0)
	    {
	        while(boolExpValue != 0)
	        {
	    	    if(*runtime.boolsettings["o"])
	            {
	                std::cout<<"Executing While loop code block:\n"<<codeBlock.data<<'\n';
	            }
	            try{
                    executionFrame.index = 0;
                    executionFrame.level = 0;
                    executionFrame.setIsFirstRun(true);
                    executionFrame.setZeroFlag(false);
                    runtime.calculateExpression(&executionFrame);
	                //runtime->callOnFrame(frame,codeBlock.data);
	            }
	            catch(std::string &exception)
	            {
	                throw std::string(exception + "\nIn While body subexp:");
	            }

	    	    if(*runtime.boolsettings["o"])
	            {
	                std::cout<<"While block Execution Complete.\n\n";
	                std::cout<<"Jumping back to execute While Boolean Expression: "<<booleanExpression.data<<"\n";
	            }
	        try{
	        	boolExecutionFrame.index = 0;
	        	boolExecutionFrame.level = 0;
	        	boolExecutionFrame.setIsFirstRun(true);
	        	boolExecutionFrame.setZeroFlag(false);
		        boolExpValue = runtime.calculateExpression(&boolExecutionFrame);

	    	    if(*runtime.boolsettings["o"])
	            {
	                if(boolExpValue)
	                {
	                    std::cout<<"Execution result true.\nContinuing loop execution.\n\n";
	                }
	                else
	                {
	                    std::cout<<"Execution result false.\nExiting loop.\n\n";
	                }
	            }
	        }
	        catch(std::string &exception)
	        {
	            throw std::string(exception  + "\nIn While Boolean Expression");
	        }
	        }
	    }
	    index = codeBlock.end + startOfCodeBlock-2;
	    frame->index = 0;
	    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
	        index++;
	    frame->index = codeBlock.start + codeBlock.end;//(index-2<frame->exp.size()?index-2:frame->exp.size());
	}
};

#endif /* KEYWORDS_WHILEACTION_H_ */
