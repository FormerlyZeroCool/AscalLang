/*
 * NewLocalVar.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWLOCALVAR_HPP_
#define KEYWORDS_NEWLOCALVAR_HPP_

#include "../Keyword.hpp"

static inline void makeArray(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
}
static inline void makeDouble(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    double value = -1;
    memcpy(&value, &ctx.frame().exp[ctx.frame().index], sizeof(double));
    ctx.frame().index += sizeof(double);
    //std::cout<<"localdouble: "<<value<<"\n";
    ctx.frame().getLocalMemory().push(StackDataRecord(StackDataRecord::DOUBLE, value));
}
static inline void makeDoubleParameter(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    double value = -1;
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->initialOperands;
    localMem.top(value);
    localMem.pop();
    //std::cout<<"paramdouble: "<<value<<" StackIndex: "<<ctx.frame().getLocalMemory().size()<<"\n";
    ctx.frame().getLocalMemory().push(StackDataRecord(StackDataRecord::DOUBLE,value));
}
static inline void makeObjectParameter(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    StackDataRecord value;
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->localMemory;
    localMem.top(value);
    localMem.pop();
    //std::cout<<"paramdouble: "<<value<<"\n";
    ctx.frame().getLocalMemory().push(value);
}
static inline void makeFunction(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    uint64_t size = -1;
    memcpy(&size, &ctx.frame().exp[ctx.frame().index], sizeof(uint64_t));
    ctx.frame().index += sizeof(uint64_t);
    const auto nameIndex = ctx.frame().index;
    ctx.frame().index += size;
    uint64_t bodySize = -1;
    memcpy(&bodySize, &ctx.frame().exp[ctx.frame().index], sizeof(uint64_t));
    ctx.frame().index += sizeof(uint64_t);
    Object *obj = ctx.runtime().memMan.constructObj(string_view(&ctx.frame().exp[nameIndex], size), 
                        string_view(&ctx.frame().exp[ctx.frame().index], bodySize));
    ctx.frame().index += bodySize;
    //std::cout<<"localobj: "<<obj->getId()<<" id: "<<ctx.frame().getLocalMemory().size()<<"\n";
    ctx.frame().getLocalMemory().push(StackDataRecord(StackDataRecord::OWNED, obj));
}
static inline void returnAndPop(KeywordExecutionContext ctx)
{
    //double data;
    //ctx.frame().initialOperands.top(data);
    ctx.runtime().frameStack.pop();
    ctx.runtime().framePool.destroy(&ctx.frame());
    ctx.runtime().frameStack.top(ctx.frame_ptr);
    //ctx.frame_ptr->initialOperands.push(data);
    //    std::cout<<data<<"\n";
}
class NewLocalVar: public StKeyword {
public:
    NewLocalVar(AscalExecutor &runtime):
    StKeyword(runtime)
    {
        this->keyWord = "loc";
    }
    void compile(CompilationContext &ctx) override
    {
        SubStrSV localName = ParsingUtil::getVarNameSV(ctx.source,ctx.source.find("loc",ctx.src_index)+4);
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
            Object newLocal = Object(runtime.memMan, string_view("", 0));

            ctx.addOwnedLocal(localName.data, this->params.statements.size());
            CompilationContext body_ctx(subexp.data, newLocal, runtime);
            //ctx.addRefedLocal(newLocal.getId(), this->params.statements.size());
            body_ctx.addRefedLocal(localName.data, this->params.statements.size());
            //this->operation = makeSelfParameter;
            //body_ctx.target.append(this->operation);
            for(int32_t i = this->params.statements.size() - 1; i >= 0; i--)
            {
                const auto &param = this->params.statements[i];
                if(param.data[0] != '&')
                {
                    body_ctx.addDoubleLocal(param.data, 0);
                }
            }
            newLocal.LexCodeAndCompile(this->runtime, body_ctx);
            //nobj = &runtime.loadUserDefinedFn(newLocal, *frame->getLocalMemory());

            this->operation = makeFunction;
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
            this->operation = returnAndPop;
            ctx.target.append(this->operation);
        }
        else
        {
            /*Object obj(runtime.memMan, localName.data);
            obj.setDouble(atof(&subexp.data[0]));
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());*/
            const uint32_t nullIndex = ParsingUtil::isNumeric(subexp.data[subexp.data.size()-1])?subexp.data.size():subexp.data.size()-1;
            
            this->operation = makeDouble;
            ctx.target.append(this->operation);
            /*ctx.target.append((uint64_t)(localName.data.size()));
            for(int i = 0; i < localName.data.size(); i++)
            {
                ctx.target.append(localName.data[i]);
            }*/
            char tmp = subexp.data[nullIndex];
            subexp.data[nullIndex] = 0;
            ctx.target.append((double) atof(&subexp.data[0]));
            subexp.data[nullIndex] = tmp;
            ctx.addDoubleLocal(localName.data, 0);
        }
    ctx.src_index = subexp.end;

    }
};

#endif /* KEYWORDS_NEWLOCALVAR_HPP_ */
