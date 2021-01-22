/*
 * ForRangeAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_FORRANGEACTION_HPP_
#define KEYWORDS_FORRANGEACTION_HPP_

#include "../Keyword.hpp"
class ForRangeAction: public Keyword {
public:
	ForRangeAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "for";
	}

	std::string action(AscalFrame<double>* frame) override
	{
	    std::string expbkp = frame->exp;
	    int index = frame->index+4;
	    while(frame->exp[index] == ' ')
	        index++;

	    SubStr itVar = ParsingUtil::getVarName(frame->exp, index);
	    const int postRangeIndex = frame->exp.find("in range", index)+8;
	    SubStr limitExpr = ParsingUtil::getExprInString(frame->exp, postRangeIndex, '(', ')', '{');
	    Object limitParams("","","");
	    limitParams.setParams(limitExpr.data);
	    if(limitParams.params.empty())
	    	throw std::string("Error no limit in for loop condition");
	    runtime->callOnFrame(frame, "loc "+itVar.data+" = 0");
	    SubStr limitStr = limitParams.params.size()>1?limitParams.params[1]:limitParams.params[0];
	    int startOfCodeBlock = limitExpr.start;
	    SubStr codeBlock("",0,0);

	    while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
	    {
	        startOfCodeBlock++;
	    }
	    index = startOfCodeBlock;
	    codeBlock = ParsingUtil::getCodeBlock(frame, index, runtime->ascal_cin);

		double i = limitParams.params.size()>1?runtime->callOnFrame(frame,limitParams.params[0].data):0;
	    if(ParsingUtil::firstChar(limitStr.data,'&'))
	    {
	    	SubStr limitPartial = ParsingUtil::getVarName(frame->exp, postRangeIndex+limitStr.start);
	    	limitStr.start = limitStr.data.find("&")+1;
	    	size_t limitStrEnd = limitStr.data.find(".")-1;


	    	Object *list = runtime->resolveNextExprSafe(frame, limitPartial);
	    	if(list->getListSize() > 0)
	    	{

	        	if(limitParams.params.size()<3 || runtime->callOnFrame(frame,limitParams.params[2].data) > 0)
	        	{
	            	for(;i < list->getListSize(); i += limitParams.params.size()>2?runtime->callOnFrame(frame,limitParams.params[2].data):1)
	            	{
	            		if(*(*boolsettings)["o"])
	                    {
	                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	                    }
	                    try{
	                        (*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
	                        runtime->callOnFrame(frame,codeBlock.data);
	                    }
	                    catch(std::string &exception)
	                    {
	                        throw std::string(exception + "\nIn for body subexp: ");
	                    }
	                    if(*(*boolsettings)["o"])
	                    {
	                        std::cout<<"for block Execution Complete.\n\n";
	                        std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<list->getListSize()<<"\n";
	                    }
	            	}
	        	}
	        	else
	        	{
	        		if(i == 0)
	        			i = list->getListSize()-1;
	            	for(;i >= 0; i += runtime->callOnFrame(frame,limitParams.params[2].data))
	            	{
	            		if(*(*boolsettings)["o"])
	                    {
	                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	                    }
	                    try{
	                        (*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
	                        runtime->callOnFrame(frame,codeBlock.data);
	                    }
	                    catch(std::string &exception)
	                    {
	                        throw std::string(exception + "\nIn for body subexp: ");
	                    }
	                    if(*(*boolsettings)["o"])
	                    {
	                        std::cout<<"for block Execution Complete.\n\n";
	                        std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<list->getListSize()<<"\n";
	                    }
	            	}
	        	}
	    	}
	    }
	    else
	    {
	        double limit = runtime->callOnFrame(frame, limitStr.data);
	    	    if(i < limit)
	    	        for(; i < limit;
	    	        		i += limitParams.params.size()>2?runtime->callOnFrame(frame,limitParams.params[2].data):1)
	    	        {

	    	        	if(*(*boolsettings)["o"])
	    	            {
	    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	    	            }
	    	            try{
	    	                (*frame->getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
	    	                runtime->callOnFrame(frame,codeBlock.data);
	    	            }
	    	            catch(std::string &exception)
	    	            {
	    	                throw std::string(exception + "\nIn for body subexp: ");
	    	            }
	    	            if(*(*boolsettings)["o"])
	    	            {
	    	                std::cout<<"for block Execution Complete.\n\n";
	    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
	    	            }
	    	        }
	    	    else
	    	        for(; i > limit;
	    	        		i += limitParams.params.size()>2?runtime->callOnFrame(frame,limitParams.params[2].data):-1)
	    	        {

	    	        	if(*(*boolsettings)["o"])
	    	            {
	    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	    	            }
	    	            try{
	    	                (*frame->getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
	    	                runtime->callOnFrame(frame,codeBlock.data);
	    	            }
	    	            catch(std::string &exception)
	    	            {
	    	                throw std::string(exception + "\nIn for body subexp: ");
	    	            }
	    	            if(*(*boolsettings)["o"])
	    	            {
	    	                std::cout<<"for block Execution Complete.\n\n";
	    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
	    	            }
	    	        }

	    }

	     runtime->callOnFrame(frame, "delete "+itVar.data);

	    index = codeBlock.end + startOfCodeBlock-2;
	    frame->index = 0;
	    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
	        index++;
	    return "a"+expbkp.substr((index-2<frame->exp.size()?index-2:frame->exp.size()),frame->exp.size());
	}
};

#endif /* KEYWORDS_FORRANGEACTION_HPP_ */
