/*
 * NewGlobalFunction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALFUNCTION_HPP_
#define KEYWORDS_NEWGLOBALFUNCTION_HPP_


#include "../Keyword.hpp"
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
            for(int32_t i = this->params.statements.size() - 1; i >= 0; i--)
            {
                const auto &param = this->params.statements[i];
                if(param.data[0] != '&')
                {
                    body_ctx.addDoubleLocal(param.data, 0);
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
        ctx.src_index = subexp.end;

    }
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
