/*
 * ForRangeAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_FORRANGEACTION_HPP_
#define KEYWORDS_FORRANGEACTION_HPP_

#include "../Keyword.hpp"
class ForRangeAction: public ComplexStKeyword {
public:
	ForRangeAction(AscalExecutor &runtime):
		ComplexStKeyword(runtime)
	{
		this->keyWord = "for";
	}

	void action(AscalFrame<double>* frame) override
	{
	    uint32_t index = frame->index+4;
	    while(frame->exp[index] == ' ')
	        index++;

	    SubStrSV itVar = ParsingUtil::getVarNameSV(frame->exp, index);
	    const int postRangeIndex = frame->exp.find("in range", index)+8;
	    SubStrSV limitExpr = ParsingUtil::getExprInStringSV(frame->exp, postRangeIndex, '(', ')', '{');
	    Object limitParams(runtime.memMan, "","","");
	    limitParams.setParams(limitExpr.data);
	    if(limitParams.params.empty())
	    	throw std::string("Error no limit in for loop condition");
        
	    SubStr limitStr = limitParams.params.size()>1?limitParams.params[1]:limitParams.params[0];
	    int startOfCodeBlock = limitExpr.start;
	    SubStr codeBlock("",0,0);

	    while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
	    {
	        startOfCodeBlock++;
	    }
	    index = startOfCodeBlock;
	    codeBlock = ParsingUtil::getCodeBlock(frame->exp, index, runtime.ascal_cin);
	    ParamFrame<double> executionFrame(runtime, frame->getParams(), frame->getParamMemory(), frame->getLocalMemory());
	    executionFrame.exp = codeBlock.data;
	    executionFrame.setIsDynamicAllocation(false);
	    executionFrame.setContext(frame->getContext());
		double i = limitParams.params.size()>1?runtime.callOnFrame(frame,limitParams.params[0].data):0;
        runtime.callOnFrame(frame, "loc "+itVar.data.str()+" = 0");
        Object *itObj = 0;
        itObj = &(*frame->getLocalMemory()).find(itVar.data);
	    if(ParsingUtil::firstChar(limitStr.data,'&'))
	    {
	    	uint32_t index = postRangeIndex+limitStr.start;
	    	SubStrSV limitPartial = ParsingUtil::getVarNameSV(frame->exp, index);
	    	limitStr.start = limitStr.data.find("&")+1;


	    	Object *list = runtime.resolveNextExprSafe(frame, limitPartial);
	    	if(list->getListSize() > 0)
	    	{
	        	if(limitParams.params.size()<3 || runtime.callOnFrame(frame,limitParams.params[2].data) > 0)
	        	{
	            	for(;i < list->getListSize(); i += limitParams.params.size()>2?runtime.callOnFrame(frame,limitParams.params[2].data):1)
	            	{
	            	    if(*runtime.boolsettings["o"])
	                    {
	                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	                    }
	                    try{
	                        //(*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
	                    	itObj->copyExceptID(list->getListElement(i, runtime.memory));
	                        executionFrame.index = 0;
	                        executionFrame.level = 0;
	                        executionFrame.setIsFirstRun(true);
	                        executionFrame.setZeroFlag(false);
	                        runtime.calculateExpression(&executionFrame);
	                    }
	                    catch(std::string &exception)
	                    {
	                        throw std::string(exception + "\nIn for body subexp: ");
	                    }
	            	    if(*runtime.boolsettings["o"])
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
	            	for(;i >= 0; i += runtime.callOnFrame(frame,limitParams.params[2].data))
	            	{
	            	    if(*runtime.boolsettings["o"])
	                    {
	                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	                    }
	                    try{
	                        //(*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
                            itObj->copyExceptID(list->getListElement(i, runtime.memory));
	                        executionFrame.index = 0;
	                        executionFrame.level = 0;
	                        executionFrame.setIsFirstRun(true);
	                        executionFrame.setZeroFlag(false);
	                        runtime.calculateExpression(&executionFrame);
	                    }
	                    catch(std::string &exception)
	                    {
	                        throw std::string(exception + "\nIn for body subexp: ");
	                    }
	            	    if(*runtime.boolsettings["o"])
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
	        double limit = runtime.callOnFrame(frame, limitStr.data);
	    	    if(i < limit)
	    	        for(; i < limit;
	    	        		i += limitParams.params.size()>2?runtime.callOnFrame(frame,limitParams.params[2].data):1)
	    	        {
	    	    	    if(*runtime.boolsettings["o"])
	    	            {
	    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	    	            }
	    	            try{
	    	                //(*frame->getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
	    	            	itObj->setDouble(i);
	                        executionFrame.index = 0;
	                        executionFrame.level = 0;
	                        executionFrame.setIsFirstRun(true);
	                        executionFrame.setZeroFlag(false);
	                        runtime.calculateExpression(&executionFrame);
	    	            }
	    	            catch(std::string &exception)
	    	            {
	    	                throw std::string(exception + "\nIn for body subexp: ");
	    	            }
	    	    	    if(*runtime.boolsettings["o"])
	    	            {
	    	                std::cout<<"for block Execution Complete.\n\n";
	    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
	    	            }
	    	        }
	    	    else
	    	        for(; i > limit;
	    	        		i += limitParams.params.size()>2?runtime.callOnFrame(frame,limitParams.params[2].data):-1)
	    	        {
	    	    	    if(*runtime.boolsettings["o"])
	    	            {
	    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
	    	            }
	    	            try{
	    	                //(*frame->getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
	    	            	itObj->setDouble(i);
	                        executionFrame.index = 0;
	                        executionFrame.level = 0;
	                        executionFrame.setIsFirstRun(true);
	                        executionFrame.setZeroFlag(false);
	                        runtime.calculateExpression(&executionFrame);
	    	            }
	    	            catch(std::string &exception)
	    	            {
	    	                throw std::string(exception + "\nIn for body subexp: ");
	    	            }
	    	    	    if(*runtime.boolsettings["o"])
	    	            {
	    	                std::cout<<"for block Execution Complete.\n\n";
	    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
	    	            }
	    	        }

	    }

	    index = codeBlock.end + startOfCodeBlock;
	    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
	        index++;
	    frame->index = (index-2<frame->exp.size()?index-2:frame->exp.size());

	}
};

#endif /* KEYWORDS_FORRANGEACTION_HPP_ */
