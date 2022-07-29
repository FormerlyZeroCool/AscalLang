/*
 * NewGlobalFunction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALFUNCTION_HPP_
#define KEYWORDS_NEWGLOBALFUNCTION_HPP_


#include "../Keyword.hpp"
static inline void convertParamToObject(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t stackIndex = 0;
    ctx.getData(stackIndex);
    ctx.index() += sizeof(stackIndex);
    //const auto lastFrame = ctx.runtime().frameStack[ctx.runtime().frameStack.size() - 2];
    ctx.frame().localMemory[stackIndex].type = StackDataRecord::REFERENCED;
    //ctx.frame().localMemory[stackIndex].data.obj = lastFrame.localMemory[ctx.frame().localMemory[stackIndex].data.integer];
    #ifdef debug
    if(*ctx.runtime().boolsettings["o"])
        std::cout<<"stack index: "<<stackIndex<<" referenced object: "<<ctx.frame().localMemory[stackIndex].data.obj->id<<"\n";
    #endif
}
static inline void convertParamToObjectAndCopy(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t stackIndex = 0;
    ctx.getData(stackIndex);
    ctx.index() += sizeof(stackIndex);
    StackDataRecord record = ctx.frame().localMemory[stackIndex];
    record.type = StackDataRecord::OWNED;
    record.data.obj = ctx.runtime().memMan.constructObj(*record.data.obj);
    #ifdef debug
    if(*ctx.runtime().boolsettings["o"])
        std::cout<<"stack index: "<<stackIndex<<" copied object: "<<ctx.frame().localMemory[stackIndex].data.obj->id<<"\n";
    #endif
}
class NewGlobalFunction: public StKeyword {
public:
    NewGlobalFunction(AscalExecutor &runtime):
    StKeyword(runtime)
    {
        this->keyWord = "let";
    }
    void compile(CompilationContext &ctx) override
    {
        SubStrSV localName = ParsingUtil::getVarNameSV(ctx.source,ctx.source.find("let",ctx.src_index)+4);
        static uint32_t startOfExp;
        if(ctx.source[localName.end + 1] == '(')
        {
            ParsingUtil::ParseStatementList(ctx.source, localName.end+1, this->params);
        }
        startOfExp = ctx.source.find('=', ctx.src_index)+1;
        const char firstCharExp = ParsingUtil::getFirstChar(ctx.source.substr(startOfExp));

        SubStrSV subexp = ParsingUtil::getExprInStringSV(ctx.source, startOfExp);

        if(firstCharExp == '[')
        {
            /*Object obj(runtime.memMan, localName.data);
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
            runtime.makeArray(*nobj);*/
        }
        else if(firstCharExp == '\"')
        {
            /*Object obj(runtime.memMan, localName.data);
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
            runtime.makeString(*nobj);*/
        }
        if(!ParsingUtil::isDouble(subexp.data))
        {
            Object &newVar = ctx.runtime.loadUserDefinedFn(Object(runtime.memMan, localName.data), ctx.runtime.memory);
            newVar.setObject();

            CompilationContext body_ctx(subexp.data, newVar, runtime);
            for(int32_t i = this->params.statements.size() - 1, j = 0; i >= 0; i--, j++)
            {
                const auto &param = this->params.statements[i];
                if(param.data[0] != '&')
                {
                    body_ctx.addDoubleLocal(param.data, 0);
                }
                else 
                {
                    operationType op = nullptr;
                    if(param.data.size() > 1 && param.data[1] != '&')
                    {
                        body_ctx.addRefedLocal(param.data.substr(1), 0);
                        op = convertParamToObject;
                    }
                    else
                    {
                        body_ctx.addOwnedLocal(param.data.substr(2), 0);
                        op = convertParamToObjectAndCopy;
                    }
                        body_ctx.append(op);
                        const uint64_t stackIndex = j;
                        body_ctx.append(stackIndex);
                }
            }

            newVar.LexCodeAndCompile(this->runtime, body_ctx);

            this->operation = Global::makeFunction;
            ctx.target.append(this->operation);
            ctx.target.append((uint64_t)(localName.data.size()));
            for(int i = 0; i < localName.data.size(); i++)
            {
                ctx.target.append(localName.data[i]);
            }
            ctx.target.append((uint64_t)(body_ctx.target.getInstructions().size() + Keyword::opcodeSize()));
            for(int i = 0; i < body_ctx.target.getInstructions().size(); i++)
            {
                ctx.target.append(body_ctx.target.getInstructions()[i]);
            }
            this->operation = Global::returnAndPop;
            ctx.target.append(this->operation);
        }
        else
        {
            /*Object obj(runtime.memMan, localName.data);
            obj.setDouble(atof(&subexp.data[0]));
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());*/
            Object &newVar = ctx.runtime.loadUserDefinedFn(Object(runtime.memMan, localName.data), ctx.runtime.memory);
            const uint32_t nullIndex = ParsingUtil::isNumeric(subexp.data[subexp.data.size()-1])?subexp.data.size():subexp.data.size()-1;
            
            this->operation = Global::makeDouble;
            ctx.target.append(this->operation);
            ctx.target.append((uint64_t)(localName.data.size()));
            for(int i = 0; i < localName.data.size(); i++)
            {
                ctx.target.append(localName.data[i]);
            }
            char tmp = subexp.data[nullIndex];
            subexp.data[nullIndex] = 0;
            const double value = atof(&subexp.data[0]);
            ctx.target.append(value);
            newVar.setDouble(value);
            subexp.data[nullIndex] = tmp;
        }
        ctx.src_index = subexp.end;

    }
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
