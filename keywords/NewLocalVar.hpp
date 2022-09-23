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
    ctx.index() += Keyword::opcodeSize();
}
static inline void makeDouble(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    double value = -1;
    ctx.getData(value);
    ctx.index() += sizeof(double);
    //std::cout<<"localdouble: "<<value<<"\n";
    ctx.localMemory().push(StackDataRecord(StackDataRecord::DOUBLE, value));
}
static inline void makeDoubleParameter(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    AscalExecutor::Operand value;
    auto &localMem = ctx.runtime().frameStack.back()->initialOperands;
    localMem.top(value);
    localMem.pop();
    //std::cout<<"paramdouble: "<<value<<" StackIndex: "<<ctx.localMemory().size()<<"\n";
    ctx.localMemory().push(StackDataRecord(StackDataRecord::DOUBLE, value.number()));
}
static inline void makeObjectParameter(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    StackDataRecord value;
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->localMemory;
    localMem.top(value);
    localMem.pop();
    //std::cout<<"paramdouble: "<<value<<"\n";
    ctx.localMemory().push(value);
}
static inline void makeFunction(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t size = -1;
    ctx.getData(size);
    ctx.index() += sizeof(uint64_t);
    const auto nameIndex = ctx.index();
    ctx.index() += size;
    uint64_t bodySize = -1;
    ctx.getData(bodySize);
    ctx.index() += sizeof(uint64_t);
    Object *obj = ctx.runtime().memMan.constructObj(string_view(&ctx.frame().exp[nameIndex], size), 
                        string_view(&ctx.frame().exp[ctx.index()], bodySize));
    ctx.index() += bodySize;
    std::cout<<"localobj: "<<obj->getId()<<" id: "<<ctx.localMemory().size()<<"\n";
    ctx.localMemory().push(StackDataRecord(StackDataRecord::OWNED, obj));
}
static inline void returnAndPop(KeywordExecutionContext ctx)
{
    //double data;
    //ctx.frame().initialOperands.top(data);
    ctx.frame().initialOperands.resetStart();
    ctx.runtime().frameStack.pop();
    ctx.runtime().framePool.destroy(&ctx.frame());
    ctx.runtime().frameStack.top(ctx.frame_ptr);
    //ctx.frame_ptr->initialOperands.push(data);
    //    std::cout<<data<<"\n";
}
inline void compileLocalFunction(CompilationContext &ctx, string_view localName, string_view subexp, ParsedStatementList &params, AscalExecutor &runtime)
{
    Object newLocal = Object(runtime.memMan, string_view("", 0));

    ctx.addOwnedLocal(localName, params.statements.size());
    CompilationContext body_ctx(subexp, newLocal, runtime);
    //ctx.addRefedLocal(newLocal.getId(), params.statements.size());
    body_ctx.addRefedLocal(localName, params.statements.size());
    //this->operation = makeSelfParameter;
    //body_ctx.target.append(this->operation);
    for(int32_t i = params.statements.size() - 1; i >= 0; i--)
    {
        const auto &param = params.statements[i];
        if(param.data[0] != '&')
        {
            body_ctx.addDoubleLocal(param.data, 0);
        }
    }
    newLocal.LexCodeAndCompile(runtime, body_ctx);
    //nobj = &runtime.loadUserDefinedFn(newLocal, *frame->getLocalMemory())
    auto operation = makeFunction;
    ctx.target.append(operation);
    ctx.target.append((uint64_t)(localName.size()));
    for(int i = 0; i < localName.size(); i++)
    {
        ctx.target.append(localName[i]);
    }
    ctx.target.append((uint64_t)(body_ctx.target.getInstructions().size() + Keyword::opcodeSize()));
    for(int i = 0; i < body_ctx.target.getInstructions().size(); i++)
    {
        ctx.target.append(body_ctx.target.getInstructions()[i]);
    }
    operation = returnAndPop;
    ctx.target.append(operation);
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
            compileLocalFunction(ctx, localName.data, subexp.data, params, runtime);
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
        
        ctx.src_index = subexp.end - 1;
        std::cout<<ctx.source.substr(ctx.src_index)<<"\n";
    }
};

#endif /* KEYWORDS_NEWLOCALVAR_HPP_ */
