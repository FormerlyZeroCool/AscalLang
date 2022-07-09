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
private:
    ParsedStatementList params;
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
        params.statements.clear();
        ParsingUtil::ParseStatementList(limitExpr.data, 0, params);
        if(params.statements.empty())
            throw std::string("Error no limit in for loop condition");
        
        SubStr limitStr = params.statements.size()>1?params.statements[1]:params.statements[0];
        int startOfCodeBlock = limitExpr.start;
        SubStr codeBlock("",0,0);

        while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
        {
            startOfCodeBlock++;
        }
        index = startOfCodeBlock;
        codeBlock = ParsingUtil::getCodeBlock(frame->exp, index, runtime.ascal_cin);
        FunctionSubFrame<double> executionFrame(runtime, frame->getParams(), frame->getParamMemory(), frame->getLocalMemory());
        executionFrame.exp = codeBlock.data;
        executionFrame.setIsDynamicAllocation(false);
        executionFrame.setContext(frame->getContext());
        double i = params.statements.size()>1?runtime.callOnFrame(frame,params.statements[0].data):0;
        Object itObjRef(runtime.memMan, itVar.data);
        frame->getParamMemory()->insert(string_view(itObjRef.id), &itObjRef);
        Object *itObj = &itObjRef;
        if(ParsingUtil::firstChar(limitStr.data,'&'))
        {
            uint32_t index = postRangeIndex+limitStr.start;
            SubStrSV limitPartial = ParsingUtil::getVarNameSV(frame->exp, index);
            limitStr.start = limitStr.data.find("&", frame->index)+1;


            Object *list = runtime.resolveNextExprSafe(frame, limitPartial);
            if(list->getListSize() > 0)
            {
                if(params.statements.size()<3 || runtime.callOnFrame(frame,params.statements[2].data) > 0)
                {
                    for(;i < list->getListSize(); i += params.statements.size()>2?runtime.callOnFrame(frame,params.statements[2].data):1)
                    {
                        if(*runtime.boolsettings["o"])
                        {
                            std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
                        }
                        try{
                            //(*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
                            if(list->isDoubleList())
                            {
                                itObj->setDouble(list->getDoubleAtIndex(i));
                            }
                            else
                                itObj = (list->getListElement(i, runtime.memory));
                            executionFrame.index = 0;
                            executionFrame.level = 0;
                            executionFrame.setIsFirstRun(true);
                            executionFrame.setZeroFlag(false);
                            runtime.calculateExpression(&executionFrame);
                            if(list->isDoubleList())
                            {
                                list->setDoubleAtIndex(i, itObj->getDouble());
                            }
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
                    for(;i >= 0; i += runtime.callOnFrame(frame,params.statements[2].data))
                    {
                        if(*runtime.boolsettings["o"])
                        {
                            std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
                        }
                        try{
                            //(*frame->getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
                            if(list->isDoubleList())
                            {
                                itObj->setDouble(list->getDoubleAtIndex(i));
                            }
                            else
                                itObj = (list->getListElement(i, runtime.memory));
                            executionFrame.index = 0;
                            executionFrame.level = 0;
                            executionFrame.setIsFirstRun(true);
                            executionFrame.setZeroFlag(false);
                            runtime.calculateExpression(&executionFrame);
                            if(list->isDoubleList())
                            {
                                list->setDoubleAtIndex(i, itObj->getDouble());
                            }
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
            itObj->setDouble(i);
            double limit = runtime.callOnFrame(frame, limitStr.data);
            bool showOps = *runtime.boolsettings["o"];
                if(i < limit)
                    for(; i < limit;
                            i += params.statements.size()>2?runtime.callOnFrame(frame,params.statements[2].data):1)
                    {
                        if(showOps)
                        {
                            std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
                        }
                        try{
                            //(*frame->getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
                            itObj->setDouble(i);
                            /*executionFrame.index = 0;
                            executionFrame.level = 0;
                            executionFrame.setIsFirstRun(true);
                            executionFrame.setZeroFlag(false);
                            runtime.calculateExpression(&executionFrame);*/
                            runtime.callOnFrame(frame, codeBlock.data);
                        }
                        catch(std::string &exception)
                        {
                            throw std::string(exception + "\nIn for body subexp: ");
                        }
                        if(showOps)
                        {
                            std::cout<<"for block Execution Complete.\n\n";
                            std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
                        }
                    }
                else
                    for(; i > limit;
                            i += params.statements.size()>2?runtime.callOnFrame(frame,params.statements[2].data):-1)
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
        frame->getParamMemory()->erase(itObj->id);
        index = codeBlock.end + startOfCodeBlock;
        while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
            index++;
        frame->index = codeBlock.start+codeBlock.end;//(index-2<frame->exp.size()?index-2:frame->exp.size());

    }
};

#endif /* KEYWORDS_FORRANGEACTION_HPP_ */
