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

    void compile(CompilationContext &ctx) override
    {
        uint32_t index = ctx.src_index+this->opcodeSize();
        while(ctx.source.size() > index && ParsingUtil::isalpha(ctx.source[index]))
            index++;
        SubStrSV itVar = ParsingUtil::getVarNameSV(ctx.source, index);
        const int postRangeIndex = ctx.source.find("range", index)+5;
        SubStrSV limitExpr = ParsingUtil::getExprInStringSV(ctx.source, postRangeIndex, '(', ')', '{');
        //std::cout<<"ivar: "<<itVar.data<< " ivar len: "<< itVar.data.length()<<" limit: "<<limitExpr.data<<"\nexpr: "<<ctx.source<<"\n";
        params.statements.clear();
        ParsingUtil::ParseStatementList(limitExpr.data, 0, params);
        if(params.statements.empty())
            throw std::string("Error no limit in for loop condition");
        
        SubStr limitStr = params.statements.size()>1?params.statements[1]:params.statements[0];
        int startOfCodeBlock = limitExpr.start;

        while(ctx.source.size() > startOfCodeBlock && ctx.source[startOfCodeBlock] != '{')
        {
            startOfCodeBlock++;
        }
        index = startOfCodeBlock;
        const SubStrSV codeBlock = ParsingUtil::getExprInStringSV(ctx.source, index);
        
        
    }
};

/*old code for action
double i = params.statements.size()>1?runtime.callOnFrame(frame,params.statements[0].data):0;
        Object itObjRef(runtime.memMan, itVar.data);
        ctx.frame.getParamMemory()->insert(string_view(itObjRef.id), &itObjRef);
        Object *itObj = &itObjRef;
        if(ParsingUtil::firstChar(limitStr.data,'&'))
        {
            uint32_t index = postRangeIndex+limitStr.start;
            SubStrSV limitPartial = ParsingUtil::getVarNameSV(ctx.source, index);
            limitStr.start = limitStr.data.find("&", ctx.src_index)+1;


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
                            //(*ctx.frame.getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
                            if(list->isDoubleList())
                            {
                                itObj->setDouble(list->getDoubleAtIndex(i));
                            }
                            else
                                itObj = (list->getListElement(i, runtime.memory));
                            executionFrame.index = 0;
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
                            //(*ctx.frame.getLocalMemory())[itVar.data] = list->getListElement(i, *memory);
                            if(list->isDoubleList())
                            {
                                itObj->setDouble(list->getDoubleAtIndex(i));
                            }
                            else
                                itObj = (list->getListElement(i, runtime.memory));
                            executionFrame.index = 0;
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
                            //(*ctx.frame.getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
                            itObj->setDouble(i);
                            executionFrame.index = 0;
                            executionFrame.level = 0;
                            executionFrame.setIsFirstRun(true);
                            executionFrame.setZeroFlag(false);
                            runtime.calculateExpression(&executionFrame);
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
                            //(*ctx.frame.getLocalMemory())[itVar.data] = Object(itVar.data,ParsingUtil::to_string(i),"");
                            itObj->setDouble(i);
                            executionFrame.index = 0;
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
        ctx.frame.getParamMemory()->erase(itObj->id);
        index = codeBlock.end + startOfCodeBlock;
        while(ctx.source[index] == ';' || ctx.source[index] == ' ' || ctx.source[index] == '}')
            index++;
        ctx.src_index = codeBlock.end;//(index-2<ctx.source.size()?index-2:ctx.source.size());

*/
#endif /* KEYWORDS_FORRANGEACTION_HPP_ */
