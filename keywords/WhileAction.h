/*
 * WhileAction.h
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHILEACTION_H_
#define KEYWORDS_WHILEACTION_H_

#include "../Keyword.hpp"
class WhileAction: public Keyword {
public:
	WhileAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "while";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    std::string expbkp = frame->exp;
	    int index = frame->index+5;
	    while(frame->exp[index] == ' ')
	        index++;

	    int startOfBoolExp = index;
	    int startOfCodeBlock = index;
	    SubStr codeBlock("",0,0);

	    while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
	    {
	        startOfCodeBlock++;
	    }

	    index = startOfCodeBlock;

	    const std::string booleanExpression = frame->exp.substr(startOfBoolExp,index-startOfBoolExp);
	    if(booleanExpression.size() == 0)
	    {
	        throw std::string("Error no boolean expression provided in while.\n");
	    }
	    double boolExpValue;
	    try{
	        if(*(*boolsettings)["o"])
	        {
	            std::cout<<"Executing While Boolean Expression: "<<booleanExpression<<"\n";
	        }
	        boolExpValue = runtime->callOnFrame(frame,booleanExpression);
	    }
	    catch(std::string &exception)
	    {
	        throw std::string(exception +"\nIn While Boolean Expression");
	    }
        if(*(*boolsettings)["o"])
	    {
	        std::cout<<"Execution Complete. "<<(boolExpValue?"true":"false")<<"\n\n";
	    }
	    codeBlock = ParsingUtil::getCodeBlock(frame, index, runtime->ascal_cin);
	    if(boolExpValue != 0)
	    {
	        while(boolExpValue != 0)
	        {

		        if(*(*boolsettings)["o"])
	            {
	                std::cout<<"Executing While loop code block:\n"<<codeBlock.data<<'\n';
	            }
	            try{
	                runtime->callOnFrame(frame,codeBlock.data);
	            }
	            catch(std::string &exception)
	            {
	                throw std::string(exception + "\nIn While body subexp:");
	            }

		        if(*(*boolsettings)["o"])
	            {
	                std::cout<<"While block Execution Complete.\n\n";
	                std::cout<<"Jumping back to execute While Boolean Expression: "<<booleanExpression<<"\n";
	            }
	        try{
	            boolExpValue = runtime->callOnFrame(frame,booleanExpression);
		        if(*(*boolsettings)["o"])
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
	    return "a"+expbkp.substr((index-2<frame->exp.size()?index-2:frame->exp.size()),frame->exp.size());
	}
};

#endif /* KEYWORDS_WHILEACTION_H_ */
