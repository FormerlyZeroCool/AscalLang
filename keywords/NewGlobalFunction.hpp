/*
 * NewGlobalFunction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALFUNCTION_HPP_
#define KEYWORDS_NEWGLOBALFUNCTION_HPP_


#include "../Keyword.hpp"
namespace Global {
static inline void makeArray(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
}
static inline void makeDouble(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    uint64_t size = -1;
    memcpy(&size, &ctx.frame().exp[ctx.frame().index], sizeof(uint64_t));
    ctx.frame().index += sizeof(uint64_t);
    const auto nameIndex = ctx.frame().index;
    ctx.frame().index += size;
    Object *obj = ctx.runtime().memMan.constructObj(string_view(&ctx.frame().exp[nameIndex], size), 
                        string_view("", 0));
    double value = -1;
    memcpy(&value, &ctx.frame().exp[ctx.frame().index], sizeof(double));
    ctx.frame().index += sizeof(double);
    obj->setDouble(value);
    ctx.runtime().memory.insert(std::make_pair(obj->getId(), obj));
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"instantiating global: "<<obj->getId()<<" to: "<<value<<"\n";
	}
	#endif
}
static inline void makeDoubleParameter(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    double value = -1;
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->initialOperands;
    localMem.top(value);
    localMem.pop();
	#ifdef debug
    std::cout<<"parameter created, double: "<<value<<"\n"<<"Var stack id: "<<ctx.frame().getLocalMemory().size();
    #endif
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
    Object obj(ctx.runtime().memMan, string_view(&ctx.frame().exp[nameIndex], size), 
                        string_view(&ctx.frame().exp[ctx.frame().index], bodySize));
    
    ctx.runtime().loadUserDefinedFn(obj, ctx.runtime().memory);
    ctx.frame().index += bodySize;
    #ifdef debug
    std::cout<<"global obj: "<<obj.getId()<<" id: "<<ctx.frame().getLocalMemory().size()<<"\n";
    #endif
    //ctx.frame().getLocalMemory().push(StackDataRecord(StackDataRecord::OWNED, obj));
}
static inline void returnAndPop(KeywordExecutionContext ctx)
{
    auto &runtime = ctx.runtime();
    ctx.runtime().frameStack.pop();
    ctx.frame().initialOperands.resetStart();
    ctx.runtime().framePool.destroy(&ctx.frame());
    runtime.frameStack.top(ctx.frame_ptr);
    //    std::cout<<data<<"\n";
}
};
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

        const uint32_t executionStart = ctx.target.getInstructions().size();
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
            Object &newLocal = ctx.runtime.loadUserDefinedFn(Object(runtime.memMan, localName.data), ctx.runtime.memory);
            newLocal.setObject();

            CompilationContext body_ctx(subexp.data, newLocal, runtime);
            //body_ctx.addRefedLocal(newLocal.getId(), this->params.statements.size());
            //body_ctx.lastVarIndex--;
            //this->operation = makeSelfParameter;
            //body_ctx.target.append(this->operation);
            for(int32_t i = this->params.statements.size() - 1; i >= 0; i--)
            {
                const auto &param = this->params.statements[i];
                std::cout<<param.data<<"\n";
                if(param.data[0] != '&')
                {
                    body_ctx.addDoubleLocal(param.data, 0);
                }
            }

            newLocal.LexCodeAndCompile(this->runtime, body_ctx);
            //nobj = &runtime.loadUserDefinedFn(newLocal, *frame->getLocalMemory());

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
            //std::cout<<"not a double not obj\n"<<"exp: "<<subexp.data<<"\n";
        }
        else
        {
            /*Object obj(runtime.memMan, localName.data);
            obj.setDouble(atof(&subexp.data[0]));
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());*/
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
            ctx.target.append((double) atof(&subexp.data[0]));
            subexp.data[nullIndex] = tmp;
        }
        const uint32_t executionEnd = ctx.target.getInstructions().size();
        string_view initGlobal = ctx.target.getInstructions().substr(executionStart, executionEnd - executionStart);
        AscalFrame<double> frame(ctx.runtime);
        frame.exp = initGlobal;
        ctx.runtime.calculateExpression(&frame);
        ctx.src_index = subexp.end;

    }
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
