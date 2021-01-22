/*
 * ifAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_IFACTION_HPP_
#define KEYWORDS_IFACTION_HPP_

#include "../Keyword.hpp"
class IfAction: public Keyword {
public:
	IfAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "if";
	}

	std::string action(AscalFrame<double>* frame) override
	{
		uint32_t index = frame->exp.find("if",frame->index)!=-1?frame->exp.find("if",frame->index)+2:0;
	    while(frame->exp[index] == ' ')
	    {
	            index++;
	    }

	        uint32_t startOfBoolExp = index;
	        uint32_t startOfCodeBlock = index;
	        SubStr codeBlock("",0,0);

	        while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
	        {
	            startOfCodeBlock++;
	        }
	        index = startOfCodeBlock;
	        const std::string booleanExpression = frame->exp.substr(startOfBoolExp,index-startOfBoolExp);
	        if(booleanExpression.size() == 0)
	        {
	            throw std::string("Error no boolean expression provided in if.\n");
	        }
	        double boolExpValue;
	        if(*(*boolsettings)["o"])
	        {
	            std::cout<<"Executing boolean expression in if: "<<booleanExpression<<'\n';
	        }
	        try{
	            boolExpValue = runtime->callOnFrame(frame,booleanExpression);
	        }
	        catch(std::string &exception)
	        {
	            throw std::string(exception + "\nIn if boolean exp: " + booleanExpression);
	        }

	        if(*(*boolsettings)["o"])
	        {
	            std::cout<<"Boolean expression evaluation complete result: "<<(boolExpValue?"true":"false")<<'\n';
	        }
	        codeBlock = ParsingUtil::getCodeBlock(frame, startOfCodeBlock, runtime->ascal_cin);

	        index = startOfCodeBlock + codeBlock.end - 1;
	        if(index > frame->exp.size())
	            index = frame->exp.size();
	        if(boolExpValue != 0)
	        {
	            if(!frame->comingfromElse() || !frame->ifResultFlag())
	            {
	                frame->setIfFlag(true);
	                frame->setIfResultFlag(true);
	            }
	            else
	            {
	                frame->setIfFlag(false);
	                frame->setIfResultFlag(true);
	            }
	            if(*(*boolsettings)["o"])
	            {
	                std::cout<<"Executing code block in if: "<<codeBlock.data<<'\n';
	            }
	            try{
	                runtime->callOnFrame(frame,codeBlock.data);
	            }
	            catch(std::string &exception)
	            {
	                throw std::string(exception + "\nIn if body subexp: "+codeBlock.data);
	            }
	            if(*(*boolsettings)["o"])
	            {
	                std::cout<<"If block Execution Complete.\n";
	            }

	        }
	        else
	        {

	        	if(*(*boolsettings)["o"])
	            {
	                std::cout<<"Skipping if code block\n";
	            }
	            if(!frame->comingfromElse() || !frame->ifResultFlag())
	            {
	                frame->setIfFlag(true);
	                frame->setIfResultFlag(false);
	            }
	            else
	            {
	                frame->setIfFlag(false);
	                frame->setIfResultFlag(false);
	            }
	        }
	        frame->setComingfromElse(false);

	        frame->index = 0;
	        return "a"+frame->exp.substr(index-1,frame->exp.size());
	}
};

#endif /* KEYWORDS_IFACTION_HPP_ */
