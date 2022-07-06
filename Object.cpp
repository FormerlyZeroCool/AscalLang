/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.hpp"
#include "AscalExecutor.hpp"
#include "CompilationContext.hpp"
#include "Keyword.hpp"

const uint8_t Object::initialOffset = sizeof(double);
    void Object::compileInstructions(AscalExecutor &runtime)
    {
        Object target(runtime.memMan);
        CompilationContext ctx(this->instructions, target, runtime);
        
        this->LexCodeAndCompile(runtime, ctx);
        
        uint8_t buffer[256];
        memset(buffer, 0, sizeof(buffer));
        memcpy(&buffer[0], &ctx.target.getInstructions()[0], 256<ctx.target.getInstructions().size()?256:ctx.target.getInstructions().size());
        int counter = 0;
        for(int j = 0; j < 16; j++){
            for(int i = 0; i < 16; i++)
            {
                printf("%02X ", buffer[counter]);
                counter++;
            }
            std::cout<<"\n";
        }
        this->deallocateInstructions(this->inp, this->instructionBufferSizeId);
        this->inp = ctx.target.inp;
        this->instructions = ctx.target.instructions;
        ctx.target.inp = nullptr;
    }
    void Object::compileInstructions(string_view source, AscalExecutor &runtime)
    {
        //this->instructions = this->compileInstructions(this->instructions, 0);
        
        CompilationContext ctx(source, *this, runtime);
        
        this->LexCodeAndCompile(runtime, ctx);
        
        //std::cout<< ctx.target.getInstructions() << "\n";
    }

    void Object::compileParams(string_view s, AscalExecutor &runtime, CompilationContext &ctx)
    {
        //std::cout<<"instruction length before param compilation: "<<ctx.target.getInstructions().size()<<"\n";
        uint32_t start = 0, end = ctx.currentToken;
        std::uintptr_t endptr = reinterpret_cast<std::uintptr_t>(&s.back());
        std::uintptr_t startptr = reinterpret_cast<std::uintptr_t>(&s[0]);
        while(ctx.lastTokens.size() > start && reinterpret_cast<std::uintptr_t>(&ctx.lastTokens[start].source.back()) < startptr)
            start++;
        end = start + 1;
        ctx.currentToken = start;
        while(ctx.lastTokens.size() > end && reinterpret_cast<std::uintptr_t>(&ctx.lastTokens[end].source.back()) <= endptr)
            end++;
        //std::cout<<"sub start: "<<ctx.currentToken<<" end: "<<(end)<<"\n";
        this->compileTokens(ctx, end);
        //std::cout<<"instruction length after param compilation: "<<ctx.target.getInstructions().size()<<"\n";
    }

    void Object::LexCodeAndCompile(AscalExecutor &runtime, CompilationContext &ctx)
    {
        //ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index], 1), ctx.src_index, CompilationContext::Token::CTXDELIMITER));
        const uint32_t tokenStart = ctx.lastTokens.size() - 1;
        //lexing
        while(ctx.src_index < ctx.source.length())
        {
            if(ParsingUtil::isalpha(ctx.source[ctx.src_index]))
            {
                SubStrSV varName = ParsingUtil::getVarNameSV(ctx.source, ctx.src_index);
                const auto keyword = runtime.inputMapper.find(varName.data);
                if(keyword != runtime.inputMapper.end())
                {
                std::cout<<"parsed keyword: \""<<varName.data<<"\"\n";
                    ctx.lastTokens.push_back(CompilationContext::Token(varName.data, varName.start, CompilationContext::Token::KEYWORD));
                    ctx.src_index += varName.data.length();
                    //ctx.lastTokens.push_back(CompilationContext::Token(varName.data, varName.start, CompilationContext::Token::KEYWORD_END));
                }
                else
                {
                    ctx.lastTokens.push_back(CompilationContext::Token(varName.data, varName.start, CompilationContext::Token::VARIABLE));
                    ctx.src_index = varName.end + 1;
                    /*for(int_fast32_t i = 0; i < varName.data.length(); i++)
                    {
                        ctx.target.append(varName.data[i]);
                        //ctx.src_index++;
                    }*/
                }
            }
            else if(ParsingUtil::isNumeric(ctx.source[ctx.src_index]) || ctx.source[ctx.src_index] == '.' ||
                (!ctx.lastTokens.size() &&  ctx.source[ctx.src_index] == '-') ||
                (ctx.lastTokens.size() && ctx.lastTokens.back().type == CompilationContext::Token::OPERATOR && ctx.lastTokens.back().source != string_view(")", 1) &&  ctx.source[ctx.src_index] == '-')
                )
            {
                int index = ctx.src_index;
                const auto number = ParsingUtil::getNextDouble(ctx.source, index);
                ctx.lastTokens.push_back(CompilationContext::Token(ctx.source.substr(ctx.src_index, index - ctx.src_index), ctx.src_index, CompilationContext::Token::NUMBER));
                //std::cout<<"source Parsed num: "<<number<<" Len: "<<(index - ctx.src_index)<<"\n";
                ctx.src_index = index;
                ctx.lastTokens.back().constantValue = number;
                //ctx.target.append(AscalExecutor::DOUBLE);
                //ctx.target.append(number);
            }
            else if(Calculator<double>::isOperator(ctx.source[ctx.src_index]))
            {
                if(ctx.source[ctx.src_index + 1] == '=' && (ctx.source[ctx.src_index] == '<' || ctx.source[ctx.src_index] == '>'))
                    ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index++], 2), ctx.src_index, CompilationContext::Token::OPERATOR));
                else if(ctx.source[ctx.src_index + 1] == '=' && ctx.source[ctx.src_index] == '!')
                    ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index++], 2), ctx.src_index, CompilationContext::Token::OPERATOR));
                else
                    ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index], 1), ctx.src_index, CompilationContext::Token::OPERATOR));
                //ctx.target.append(AscalExecutor::OPERATOR);
                //ctx.target.append(ctx.source[ctx.src_index]);
                ctx.src_index++;
            }
            else if(ctx.source[ctx.src_index] == ';' || ctx.source[ctx.src_index] == '\n')
            {
                //ctx.lastTokens.clear();
                ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index], 1), ctx.src_index, CompilationContext::Token::DELIMITER));
                //ctx.target.append(AscalExecutor::DELIMITER);
                //ctx.target.append(ctx.source[ctx.src_index]);
                ctx.src_index++;
            }
            else if(ctx.source[ctx.src_index] == '#')
            {
                while(ctx.src_index < ctx.source.length() && ctx.source[ctx.src_index++] != '\n') {}
            }
            else if(ctx.source[ctx.src_index] == '&' ||ctx.source[ctx.src_index] == '[' || ctx.source[ctx.src_index] == ']' || ctx.source[ctx.src_index] == '"' || ctx.source[ctx.src_index] == '.')
            {
                ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index], 1), ctx.src_index, CompilationContext::Token::VARIABLE_PART));
                
                //std::cout<<"source Parsed char: "<<ctx.source[ctx.src_index]<<" ind: "<<(ctx.src_index)<<"\n"<<"isnum: "<<ParsingUtil::isNumeric(ctx.source[ctx.src_index])<<"\n";
                //ctx.target.append(ctx.source[ctx.src_index++]);
                ctx.src_index++;
            }
            else
            {
                ctx.lastTokens.push_back(CompilationContext::Token(string_view(&ctx.source[ctx.src_index], 1), ctx.src_index, CompilationContext::Token::null));
                
                //std::cout<<"source Parsed char: "<<ctx.source[ctx.src_index]<<" ind: "<<(ctx.src_index)<<"\n"<<"isnum: "<<ParsingUtil::isNumeric(ctx.source[ctx.src_index])<<"\n";
                //ctx.target.append(ctx.source[ctx.src_index++]);
                ctx.src_index++;
            }
        }
        ctx.currentToken = tokenStart + 1;
        //code generation
        this->compileTokens(ctx, std::numeric_limits<uint32_t>::max());
    }
    void globalVariableDoubleArrayAccess(KeywordExecutionContext ctx)
    {
        ctx.frame().index += Keyword::opcodeSize();
        Object *obj = nullptr;
        double index = 0; 
        memcpy(&obj, &ctx.frame().exp[ctx.frame().index], sizeof(Object*));
        ctx.frame().index += sizeof(Object*);
        memcpy(&index, &ctx.frame().exp[ctx.frame().index], sizeof(double));
        ctx.frame().index += sizeof(double);
        ctx.frame().initialOperands.push(obj->getDoubleAtIndex(index));
    }
    void globalVariableDoubleAccess(KeywordExecutionContext ctx)
    {
        ctx.frame().index += Keyword::opcodeSize();
        Object *obj = nullptr;
        memcpy(&obj, &ctx.frame().exp[ctx.frame().index], sizeof(Object*));
        ctx.frame().index += sizeof(Object*);
        ctx.frame().initialOperands.push(obj->getDouble());
    }
    void functionCallSetup(KeywordExecutionContext ctx)
    {
        Object *obj = nullptr;
        size_t paramsCount = 0;
        ctx.getData(obj, ctx.frame().index + sizeof(Object*));
        ctx.getData(paramsCount, ctx.frame().index + sizeof(Object*) * 2);
        ctx.frame().index += sizeof(size_t) + sizeof(Object*) * 2;
        auto newFrame = ctx.runtime().framePool.construct(ctx.runtime());
        double var;
        for(int i = 0; i < paramsCount; i++)
        {
            ctx.frame().initialOperands.top(var);
            ctx.frame().initialOperands.pop();
            newFrame->localMemory.push(StackDataRecord(StackDataRecord::DOUBLE, var));
        }
        newFrame->initialOperands.resetStart();
        newFrame->exp = obj->getInstructions();
        newFrame->index = 0;
        ctx.runtime().frameStack.push(newFrame);
        ctx.frame_ptr = newFrame;
        #ifdef debug
        std::cout<<" obj*: "<<obj<<" frame ptr: "<<newFrame->exp.length()<<"\n";
        #endif
    }
    
    
    inline void plusOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b+a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"+"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void minusOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b-a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"-"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void multiplyOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b*a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"*"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void divisionOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b/a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"/"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void modOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(doubleModulus(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"%"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void exponentOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(std::pow(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"^"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void permuteOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(permute(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"P"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void combinationsOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(combinations(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"C"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void logOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(log(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<"log"<<b<<"("<<a<<")"<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void rootOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(rootOp(b,a));
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"$"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void equalsOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b == a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"="<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void lessThanOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b < a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"<"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void greaterThanOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b > a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<">"<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void lessThanOrEqualToOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b <= a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<"<="<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void greaterThanOrEqualToOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b >= a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<">="<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    inline void notEqualToOp(KeywordExecutionContext ctx)
    {
        double a = -1, b = -1;
        ctx.frame().initialOperands.top(a);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.top(b);
        ctx.frame().initialOperands.pop();
        ctx.frame().initialOperands.push(b >= a);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<b<<">="<<a<<"\n";
        #endif
        ctx.frame().index += Keyword::opcodeSize();
    }
    void compileOperator(CompilationContext &ctx, StackSegment<CompilationContext::Token> &stack)
    {
        operationType op = noop;
        
        CompilationContext::Token thisOp;
        stack.top(thisOp);
        if(thisOp.source == string_view("+", 1))
        {
            op = plusOp;
        }
        else if(thisOp.source == string_view("-", 1))
        {
            op = minusOp;
        }
        else if(thisOp.source == string_view("*", 1))
        {
            op = multiplyOp;
        }
        else if(thisOp.source == string_view("/", 1))
        {
            op = divisionOp;
        }
        else if(thisOp.source == string_view("%", 1))
        {
            op = modOp;
        }
        else if(thisOp.source == string_view("^", 1))
        {
            op = exponentOp;
        }
        else if(thisOp.source == string_view("P", 1))
        {
            op = permuteOp;
        }
        else if(thisOp.source == string_view("C", 1))
        {
            op = combinationsOp;
        }
        else if(thisOp.source == string_view("@", 1))
        {
            op = logOp;
        }
        else if(thisOp.source == string_view("$", 1))
        {
            op = rootOp;
        }
        else if(thisOp.source == string_view("=", 1))
        {
            op = equalsOp;
        }
        else if(thisOp.source == string_view("<=", 2))
        {
            op = lessThanOrEqualToOp;
        }
        else if(thisOp.source == string_view(">=", 2))
        {
            op = greaterThanOrEqualToOp;
        }
        else if(thisOp.source == string_view("!=", 2))
        {
            op = notEqualToOp;
        }
        else if(thisOp.source == string_view("<", 1))
        {
            op = lessThanOp;
        }
        else if(thisOp.source == string_view(">", 1))
        {
            op = greaterThanOp;
        }
        
        ctx.target.append(op);
    }
    void localFunctionCall(KeywordExecutionContext ctx)
    {
        uint64_t value = -1;
        ctx.getData(value, ctx.frame().index + sizeof(uint64_t));

        //std::cout<<"Loading frame for fun: "<<value;
        uint64_t paramsCount;
        ctx.getData(paramsCount, ctx.frame().index + sizeof(uint64_t) * 2);
        //std::cout<<"stack index value: "<<value<<" paramsCount: "<<paramsCount<<"\n";
        ctx.frame().index += sizeof(uint64_t) * 3;
        Object *obj = (ctx.frame().localMemory)[value].data.obj;

        auto newFrame = ctx.runtime().framePool.construct(ctx.runtime());
        newFrame->exp = obj->getInstructions();
        newFrame->index = 0;
        newFrame->localMemory.push(StackDataRecord(StackDataRecord::REFERENCED, obj));
        for(int i = 0; i < paramsCount; i++)
        {
            double data;
            ctx.frame().initialOperands.top(data);
            ctx.frame().initialOperands.pop();
            newFrame->localMemory.push(StackDataRecord(StackDataRecord::DOUBLE, data));
        }

        //std::cout<<" obj*: "<<obj<<" frame ptr: "<<newFrame->exp.length()<<"\n";
        ctx.runtime().frameStack.push(newFrame);
        ctx.frame_ptr = newFrame;
    }
    void Object::compileTokens(CompilationContext &ctx, uint32_t end)
    {
        end = end < ctx.lastTokens.size() ? end : ctx.lastTokens.size();
        StackSegment<CompilationContext::Token> stack = ctx.tokenStack;
        stack.push(CompilationContext::Token(string_view("(", 1), ctx.src_index, CompilationContext::Token::OPERATOR));
        const auto &tokens = ctx.lastTokens;
        for(uint32_t &i = ctx.currentToken; i < end; i++)
        {
            const auto token = tokens[i];
            //std::cout<<"current token id: "<<ctx.currentToken<<"\n"<<"type: "<<token.type<<"\n";
            if(token.type == CompilationContext::Token::NUMBER)
            {
                //std::cout<<"loading num: "<<token.source<<"\n";
                operationType op = readAndPushDouble;
                ctx.target.append(op);
                ctx.target.append(token.constantValue);
            }
            else if(token.type == CompilationContext::Token::VARIABLE)
            {
                auto endIt = tokens.begin() + i;
                while(endIt != tokens.end() && (endIt->type == CompilationContext::Token::VARIABLE || endIt->type == CompilationContext::Token::VARIABLE_PART 
                    || endIt->type == CompilationContext::Token::NUMBER))
                    endIt++;
                Object *parent = nullptr;
                const auto it = ctx.runtime.memory.find(token.source);
                const auto localIt = ctx.localMemory.find(token.source);
                //std::cout<<"parsing varname: "<<token.source<<"\n";
                if(it != ctx.runtime.memory.end() && localIt == ctx.localMemory.end())
                {
                    Object *current = (*it).getValue();
                    if(current->isDouble())
                    {
                        
                        operationType GlobalVariableDoubleAccess = globalVariableDoubleAccess;
                        ctx.target.append(GlobalVariableDoubleAccess);
                        ctx.target.append(current);
                        endIt = tokens.begin() + i + 1;
                    }
                    else if(!current->isList() && current->getInstructions().length() > 1)//is function
                    {
                        endIt = tokens.begin() + i + 1;
                        if(endIt != tokens.end() && endIt->source[0] == '(')
                        {
                            uint32_t paramsStartSource = ctx.getSrcIndex(*endIt);

	                        SubStrSV exp = ParsingUtil::getFollowingExprSV(ctx.source, paramsStartSource, string_view("", 0));
                                //std::cout<<"param op: "<<exp.data<<"\n";
	                        ParsingUtil::ParseStatementList(exp.data,0,ctx.runtime.paramsBuffer);
                            
                            for(uint32_t j = 0; j < ctx.runtime.paramsBuffer.statements.size(); j++)
                            {
                                ctx.target.compileParams(ctx.runtime.paramsBuffer.statements[j].data, ctx.runtime, ctx);
                            }
                        }
                        
                        operationType FunctionCallSetup = functionCallSetup;
                        ctx.target.append(FunctionCallSetup);
                        ctx.target.append(current);
                        ctx.target.append((size_t) ctx.runtime.paramsBuffer.statements.size());
                    }
                    else
                    for(auto it = tokens.begin() + i + 1; it < endIt; it++)
                    {
                        const auto &varToken = *it;
                        if(varToken.type == CompilationContext::Token::VARIABLE)
                        {
                            /*for(int_fast32_t j = 0; j < varToken.source.length(); j++)
                            {
                                ctx.target.append(varToken.source[j]);
                            }*/
                            const auto node = current->objectMap.search(varToken.source);
                            if(node)
                            {
                                parent = current;
                                current = node->data.second;
                            }
                        }
                        else if(token.type == CompilationContext::Token::NUMBER)
                        {
                            
                            operationType op = readAndPushDouble;
                            ctx.target.append(op);
                            ctx.target.append(varToken.constantValue);
                            if(current->isDoubleList())
                            {
                                
                                operationType GlobalVariableDoubleArrayAccess = globalVariableDoubleArrayAccess;
                                ctx.target.append(GlobalVariableDoubleArrayAccess);
                                ctx.target.append(current);
                                ctx.target.append(token.constantValue);
                                break;
                            }
                            else if(current-isObjList())
                            {

                            }
                        }
                        else
                        {
                            //ctx.target.append(varToken.source[0]);
                        }
                }
                
                }
                else if(localIt != ctx.localMemory.end())
                {
                    const auto current = (*localIt).getValue();

                        //std::cout<<"local variable compiliation\n";
                    if(current.isObject())
                    {
                        //make sure to have params executed and pushed to results stack beforehand
                        //create frame push paramsCount params to data stack from parent function
                        endIt = tokens.begin() + i + 1;
                        //std::cout<<"next token after local: "<<endIt->source<<"\n";
                        if(endIt != tokens.end() && endIt->source[0] == '(')
                        {
                            ctx.src_index = endIt->start;
                            SubStrSV exp = ParsingUtil::getFollowingExprSV(ctx.source, ctx.src_index, string_view("", 0));
                                //std::cout<<"param op: "<<exp.data<<"\n";
                            ParsingUtil::ParseStatementList(exp.data,0,ctx.runtime.paramsBuffer);
                            int32_t j = ctx.runtime.paramsBuffer.statements.size() - 1;
                            auto ctBackup = i;
                            if(j >= 0)
                            {
                                ctx.target.compileParams(ctx.runtime.paramsBuffer.statements[j].data, ctx.runtime, ctx);
                                j--;
                                ctBackup = i+1;
                            }
                            for(; j >= 0; j--)
                            {
                                ctx.target.compileParams(ctx.runtime.paramsBuffer.statements[j].data, ctx.runtime, ctx);
                            }
                            ctx.currentToken = ctBackup;
                        }
                        operationType opLocalFunctionCall = localFunctionCall;
                        ctx.target.append(opLocalFunctionCall);
                        ctx.target.append((uint64_t) current.stack_index);
                        ctx.target.append((uint64_t) ctx.runtime.paramsBuffer.statements.size());
                        //std::cout<<"loading local function call\n";
                    }
                    else if(current.isDouble())
                    {
                        //std::cout<<"loading local variable load double\nstack index: "<<current.stack_index<<"\n";
                        operationType opLocalDoubleVarRead = localDoubleVarRead;
                        ctx.target.append(opLocalDoubleVarRead);
                        ctx.target.append((uint64_t) current.stack_index);
                        
                        i += endIt - (tokens.begin() + i) - 1;
                    }
                }
            }
            else if(token.type == CompilationContext::Token::OPERATOR && token.source[0] == ')')
            {
                //std::cout<<"handling op: "<<token.source<<"\n";
                CompilationContext::Token nextOp;
                stack.top(nextOp);
                while(!stack.isEmpty() && nextOp.source[0] != '(')
                {
                    //std::cout<<"should not be (: "<<nextOp.source<<"\n";
                    //to do
                    compileOperator(ctx, stack);
                    stack.pop();
                    stack.top(nextOp);
                }
                if(!stack.isEmpty()) {
                    stack.top(nextOp);
                    //std::cout<<"should be (: "<<nextOp.source<<"\n";
                    stack.pop();
                }
                //stack.push(token);
            }
            else if(token.type == CompilationContext::Token::OPERATOR)
            {
                //std::cout<<"loading op: "<<token.source<<"\n";
                CompilationContext::Token nextOp;
                stack.top(nextOp);
                if(token.source[0] != '(')
                while(!stack.isEmpty() && Calculator<double>::getPriority(nextOp.source[0]) >= Calculator<double>::getPriority(token.source[0]) && nextOp.source[0] != '(')
                {   
                    //to do
                    compileOperator(ctx, stack);
                    stack.pop();
                    stack.top(nextOp);
                }
                stack.push(token);
            }
            else if(token.type == CompilationContext::Token::KEYWORD)
            {
                //std::cout<<"loading keyword: "<<token.source<<"\n";
                //string_view source = token.source;
                //source.resize(strlen(source.c_str()));
                const auto keyword = ctx.runtime.inputMapper.find(token.source);
                ctx.src_index = token.start;
                (*keyword).getValue()->compile(ctx);
                //std::cout<<"keyword: "<<token.source<<" compiled.\n";
                ctx.syncTokenIndexToSrcIndex();
                //if(tokens.size() > i)
                  //  std::cout<<"nextToken: "<<tokens[i].source<<"\n";
            }
            else if(token.type == CompilationContext::Token::DELIMITER)
            {
		        CompilationContext::Token nextOp;
                stack.top(nextOp);
                if(token.source[0] != '(')
                while(!stack.isEmpty() && Calculator<double>::getPriority(nextOp.source[0]) >= Calculator<double>::getPriority(token.source[0]) && nextOp.source[0] != '(')
                {   
                    //to do
                    compileOperator(ctx, stack);
                    stack.pop();
                    stack.top(nextOp);
                }
		        operationType cs = clearStackExcept1;
		        ctx.target.append(cs);
            }
            else
            {
                //std::cout<<"loading junk: "<<token.source<<"\n";

                //ctx.target.append(token.source[0]);
            }
        }
                //std::cout<<"finishing exp: "<<ctx.source<<"\n";
                CompilationContext::Token nextOp;
                stack.top(nextOp);
                while(!stack.isEmpty() && nextOp.source[0] != '(')
                {
                    //std::cout<<"should not be (: "<<nextOp.source<<"\n";
                    compileOperator(ctx, stack);
                    stack.pop();
                    stack.top(nextOp);
                }
                //not needed because of stack's destructor
                if(!stack.isEmpty()) {
                    stack.top(nextOp);
                    //std::cout<<"should be (: "<<nextOp.source<<"\n";
                    stack.pop();
                }
        
    }
    struct codeRec {
        string_view boolExp;
        std::string codeBlock;
    };
void newLine(std::stringstream &data,int indentationLevel)
{
    data<<('\n');
    for(int i = 0;i < indentationLevel;i++)
    {
        data<<(' ');
        data<<(' ');
    }
}

std::string Object::instructionsToFormattedString() const
{
    return instructionsToFormattedString(1);
}
std::string Object::instructionsToFormattedString(uint16_t indentationLevel) const
{
    std::stringstream data;

    //newLine(data,indentationLevel);
    //indentationLevel++;
    if(this->instructions[0] != 5 && this->listSize == 0)
        for(size_t i = 0; i < instructions.size(); i++)
        {
            const char &c = this->instructions[i];
            if(c == '{'){
                indentationLevel++;
            }
            else if(c == '}')
            {
                indentationLevel--;
                newLine(data,indentationLevel);
            }

            if(c == ';')
            {
                data<<(c);
                newLine(data,indentationLevel);
            }
            else if(c == 1)
            {
                i += sizeof(double);
                double d;
                memcpy(&d, &this->instructions[Object::initialOffset], sizeof(double));
                data << ParsingUtil::to_string(d);
            }
            else
                data<<(c);
            if(c == '{')
                newLine(data,indentationLevel);
        }
    else {
        data<<"[]";
        newLine(data,indentationLevel);
    }
        newLine(data,indentationLevel);

    return data.str();
}

std::string Object::toString(uint16_t depth)
{
    std::stringstream s;
    newLine(s, depth);
    s<<"{";
    depth++;
    newLine(s, depth);
    s<<"Obj Name: ("<<this->id<<")"<<this;
    newLine(s, depth);
    s<<"Instructions: "<<this->instructionsToFormattedString(depth);
    if(this->getListSize())
    {
        depth++;
        newLine(s, depth);
        s<<"List len: "<<this->getListSize();
        s<<" data: [";
        depth++;
        for(size_t i = 0; i < this->getListSize(); i++)
        {
            newLine(s, depth);
            if(this->isObjList()){
                auto &obj = this->getObjectAtIndex(i);
                s<<obj.toString(depth);
            }
            else
            {
                s<<this->getDoubleAtIndex(i);
            }
        }
        depth--;
        newLine(s, depth);
        s<<"]";
        depth--;
        newLine(s, depth);
    }
    if(this->objectMap.size()>0)
    {
        depth++;
        newLine(s, depth);
        s<<"Mapped data: {";
        depth++;
        for(auto &value:this->objectMap)
        {
            newLine(s, depth);
            s<<value.toString(depth);
        }
        depth--;
        newLine(s, depth);
        s<<"}";
        depth--;
        newLine(s, depth);
    }
    depth--;
    newLine(s, depth);
    s<<"}";
    return s.str();
}
bool cmpVector(const std::vector<std::string>& a,const std::vector<std::string>& b)
{
    bool equal = true;
    if(a.size() != b.size())
        equal = false;
    else
    {
        for(int i = 0; i < a.size() && equal;i++)
        {
            if(a[i].compare(b[i]) != 0)
                equal = false;
        }
    }
    return equal;
}
bool Object::operator==(const Object &o) const
{
    bool equal = (id == o.id);
    return equal;
}
bool Object::operator==(Object &o) const
{
    bool equal = (id == o.id);
    return equal;
}

bool Object::isList()
{
    return this->listSize;
}

bool Object::isObjList()
{
    return (this->flagRegisters & 2) == 2;
}
size_t Object::getListSize() const
{
    return this->listSize;
}
Object* Object::getListElement(size_t index, FlatMap<string_view, Object*> &memory)
{
    if(index < this->getListSize())
        return &(*this)[index];
    return memory[string_view("null")];
}
Object& Object::operator[](size_t index)
{
    if(this->listSize > index){
        return getObjectAtIndex(index);
    }
    else
        throw std::string("Invalid index: "+std::to_string(index)+" in object: "+this->toString());
}

void Object::pushList(Object &&data)
{
    this->pushList(data);
}
Object& Object::setList(Object &data, size_t index)
{
    if(this->listSize < index)
        throw std::string("Out of range in list: "+this->id.str());
    else if(this->listSize == index)
        this->pushList(data);
    else
        (*this)[index] = data;
    return data;
}


Object& Object::getMapUnsafe(string_view id)
{
    return this->objectMap[id];
}
Object& Object::operator[](string_view id)
{
    if(this->objectMap.count(id) > 0)
        return this->objectMap[id];
    else
        throw std::string("Error invalid field ("+id.str()+") in object "+this->id.str());
}
std::string Object::listToString(FlatMap<string_view, Object*> &memory)
{
    
    if(this->isList())
    {
        std::string str;
        for(size_t i = 0; i < this->getListSize(); i++)
        {
            char letter = (char) this->getDoubleAtIndex(i);
            if(isprint(letter))
                str += letter;
        }
        return str;
    }
    else if(this->isDouble())
    {
        return ParsingUtil::to_string(this->getDouble());
    }
    else if(this->isDoubleList())
    {
        std::string str;
        for(int i = 0; i < this->getListSize(); i++)
        {
            str += this->getDoubleAtIndex(i);
        }
        return str;
    }
    else
    {
        std::string str;
        for(int i = 0; i < this->getListSize(); i++)
        {
            str += this->getObjectAtIndex(i).listToString(memory);
        }
        return str;
    }
}
void Object::printList(FlatMap<string_view, Object*> &memory)
{
    for(size_t i = 0; i < this->getListSize(); i++)
    {
        std::cout<<(char) this->getDoubleAtIndex(i);
    }
}
void Object::loadString(string_view s)
{
    clearList();
    uint16_t last = s[0],cur = s[0];
    for(uint32_t i = 1; i < s.size(); i++)
    {
        last = cur;
        cur = s[i];
        if(last == '\\' && cur == 'n'){
            this->pushList((double) '\n');
            if(i+1<s.size())
                cur = s[++i];
        }
        else
            this->pushList((double) last);
    }
    if(cur != '\"' && (last != '\\' || cur != 'n'))
        this->pushList((double) cur);

}
    
Object Object::subString(uint_fast64_t start, uint_fast64_t length, FlatMap<string_view, Object*> &)
{
    Object subString(this->objectMap.getMemMan(), id+"Sub","","");
    if(this->isDoubleList())
    {
        for(uint_fast64_t i = start; i < start+length; i++)
        {
            subString.pushList(this->getDoubleAtIndex(i));
        }
    }
    else if(this->isObjList())
    {
        for(uint_fast64_t i = start; i < start+length; i++)
        {
            subString.pushList(this->getObjectAtIndex(i));
        }
    }
    return subString;
}
Object Object::splitString(string_view filter, FlatMap<string_view, Object*> &memory)
{
    Object splitArr(this->objectMap.getMemMan(), id+"Split","","");
    size_t start = 0, end = 0;
    std::stringstream data;
    for(size_t i = 0; i < this->getListSize(); i++)
    {
        //just getting double will probably be a bug unless we make sure every variable even in lists is in double form
        //temporary fix to get compiling for testing other issues need to revise
        data<<(char) (this->getDoubleAtIndex(i));
    }
    std::string datastr = data.str();
    string_view dataAsSV(datastr);
    // search for pattern
    // set end to index returned -1
    // load string
    while(end < datastr.size())
    {
        end = dataAsSV.find(filter, start)-1;
        end = end<=datastr.size()?end:datastr.size();
        Object element(this->objectMap.getMemMan(), "","","");
        element.loadString(string_view((char*)datastr.c_str()+start, end));
        splitArr.pushList(element);
        start = end+filter.size();
        end = start;
    }
    return splitArr;
}
/*
int Object::setParams(string_view param, uint32_t start)
{
    params.clear();
    uint32_t end = start,startBackup = start;
    while(param.size() > start && param[start] != '(')// && !isalpha(param[start]) && param[start] != '-' && param[start] != '&' && !(param[start] < 58 && param[start]> 47))
    {
        ++start;
        ++end;
    }

    start += param.size() > start;
    end += param.size() > start;
    int pCount = param.size() > start;
    bool foundClosing = !pCount;
    while(!foundClosing && param.size() > end)
    {
        pCount += (param[end] == '(') - (param[end] == ')');
        if(pCount == 0)
        {
            foundClosing = true;
        }
        if((param[end] == ',' || foundClosing) && end > start && (pCount == 1 || (pCount == 0 && foundClosing)))
        {
            params.push_back(SubStrSV(param.substr(start,end-start),start, end));
                start = end;
                while(param[start] == ',' || (!isalpha(param[start]) && param[start] != '-' && !(param[start] < 58 && param[start > 47])))
                {
                    start++;
                    end++;
                }

        }

        end++;
    }
    return ++end-startBackup;
}*/
void Object::setParent(Object *p)
{
    this->parent = p;
}
Object& Object::loadChild(Object &data, AscalExecutor &runtime)
{
    data.parent = this;
    return runtime.loadUserDefinedFn(data, this->objectMap);
}

void Object::eraseList(long index)
{
    if(index >= 0 && index < this->getListSize())
    {
        if(this->isDoubleList() || this->isObjList())
        {
            void* element = reinterpret_cast<void*>(&this->instructions[initialOffset + index * sizeof(uint64_t)]);
            void* finalElement = reinterpret_cast<void*>(&this->instructions[this->instructionBufferSizeId - 1 * sizeof(uint64_t)]);
            const long elementsToMove = reinterpret_cast<uint64_t*>(finalElement) - reinterpret_cast<uint64_t*>(element);

            if(this->isObjList())
            {
                this->objectMap.getMemMan().obj_free(reinterpret_cast<Object*>(element));
                memcpy(element, reinterpret_cast<Object**>(element) + 1, elementsToMove * sizeof(uint64_t));
            }
            else
                memcpy(element, reinterpret_cast<double*>(element) + 1, elementsToMove * sizeof(uint64_t));
            this->listSize--;
        }
    }
    else
        throw std::string("Error deleting element, no element at index: ") + ParsingUtil::to_string(index);
}
Object* Object::getThis()
{
    return this->parent;
}
std::string Object::toString()
{
    return toString(0);
}
void Object::deallocateId(void *ptr, size_t bufSize)
{
    bufSize &= 0U - (ptr != nullptr);
    switch (bufSize){
        case (SMALL_ID):
            this->objectMap.getMemMan().small_id_free(ptr);
            break;
        case (LARGE_ID):
            this->objectMap.getMemMan().large_id_free(ptr);
            break;
        case (0):
            break;
        default:
            free(ptr);
    }
}
void Object::deallocateInstructions(void *ptr, size_t bufSize)
{
    bufSize &= 0U - (ptr != nullptr);
    switch (bufSize) {
        case (SMALL_EXP):
            this->objectMap.getMemMan().small_free(ptr);
            break;
        case (MEDIUM_EXP):
            this->objectMap.getMemMan().medium_free(ptr);
            break;
        case (LARGE_EXP):
            this->objectMap.getMemMan().large_free(ptr);
            break;
        case (VERYLARGE_EXP):
            this->objectMap.getMemMan().vlarge_free(ptr);
            break;
        case (0):
            break;
        default:
            free(static_cast<void*>(ptr));
            break;
    };
}
void Object::deallocateMemory(void* ptr, const size_t bufSize, void *idptr, const uint32_t idbufSize)
{
    deallocateId(idptr, idbufSize);
    deallocateInstructions(ptr, bufSize);
}

bool Object::isDoubleList()
{
    return this->instructions[0] == 6;
}

void Object::pushList(double data)
{
    static const uint16_t objSize = sizeof(double);
    if(initialOffset+this->listSize*(objSize) < this->instructionBufferSizeId)
    {
        this->flagRegisters = 1;
        memcpy(&this->instructions[initialOffset+this->listSize*(objSize)], &data, objSize);
        listSize++;
    }
    else
    {
        this->resizeInstructionsCopyDealloc();
        pushList(data);
    }/*
    else
    {
        Object obj(this->objectMap.getMemMan(), std::to_string(listSize)+this->id.str());
        obj.setDouble(data);
        this->pushList(obj);
    }*/
}
void Object::pushList(Object &data)
{
    static const uint16_t objSize = sizeof(Object*);
    if(initialOffset+this->listSize*(objSize) < this->instructionBufferSizeId)
    {
        this->flagRegisters = 2;
        data.id = std::to_string(listSize)+this->id.str();
        data.parent = this;
        Object *obj = this->objectMap.getMemMan().constructObj(data);
        memcpy(&this->instructions[initialOffset+this->listSize*(objSize)], &obj, objSize);
        listSize++;
    }
    else
    {
        if(this->instructionBufferSizeId < data.instructions.size() + this->instructions.size())
        {
            this->instructions.resize(this->instructions.size() + data.instructions.size());
        }
        else
        {
            this->resizeInstructionsCopyDealloc();
        }
        pushList(data);
    }
}
void Object::resizeInstructions(uint32_t newSize)
{
    if(newSize <= MEDIUM_EXP)
    {
        this->instructionBufferSizeId = MEDIUM_EXP;
        this->inp = this->objectMap.getMemMan().medium_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else if(newSize <= LARGE_EXP)
    {
        this->instructionBufferSizeId = LARGE_EXP;
        this->inp = this->objectMap.getMemMan().large_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else if(newSize <= VERYLARGE_EXP)
    {
        this->instructionBufferSizeId = VERYLARGE_EXP;
        this->inp = this->objectMap.getMemMan().vlarge_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else
    {
        this->instructionBufferSizeId = newSize * 2;
        this->inp = malloc(newSize * 2);
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
}
Object& Object::copyExceptID(const Object& o)
{
    copyToInstructions(o.instructions);
    this->objectMap = o.objectMap;
    this->parent = o.parent;
    return *this;
}
void Object::copyToId(string_view id)
{
    if(id.size() <= this->IdBufferSizeId)
    {
        memcpy(&this->id[0], &id[0], id.size());
        this->id.resize(id.size());
    }
    else
    {
        void *ptr = this->idp;
        const uint32_t bufSize = this->IdBufferSizeId;
        this->loadId(id);
        this->deallocateId(ptr, bufSize);
    }
}
void Object::copyToInstructions(string_view ins)
{
    if(this->instructionBufferSizeId >= ins.size())
    {
        memcpy(&this->instructions[0], &ins[0], ins.size());
        this->instructions.resize(ins.size());
    }
    else
    {
       void *ptr = this->inp;
        const uint32_t bufSize = this->instructionBufferSizeId;
        this->loadInstructions(ins);
        this->deallocateInstructions(ptr, bufSize);
    }
}
void Object::clone(const Object &o)
{
    loadData(o.id, o.instructions);
    if(this->isObjList())
    {
        for(int_fast64_t i = 0; i < o.getListSize(); i++)
        {
            Object &toCopy = o.getObjectAtIndex(i);
            char *location = &this->instructions[initialOffset+i*(sizeof(uint64_t))];
            Object *newObj = this->objectMap.getMemMan().constructObj(toCopy);
            memcpy(&location, &newObj, sizeof(Object*));
        }
    }
    this->objectMap = o.objectMap;
    this->listSize = o.getListSize();
}
Object& Object::operator=(const Object &o)
{
    this->clone(o);
    return *this;
}
//need to make a copy constructor for the objectMap
Object::Object(const Object &o): objectMap(o.objectMap)
{
    this->clone(o);
}
Object::Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &&id,const std::string &expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
    //instructions = compileInstructions(0);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &&expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan): objectMap(memMan)
{
    loadData(std::string("                "),std::string(""));
}
Object::Object(MemoryManager &memMan, string_view id): objectMap(memMan)
{
    char c = 0;
    string_view exp(&c, 0);
    loadData(id, exp);
}
Object::Object(MemoryManager &memMan, string_view id, string_view exp): objectMap(memMan)
{
    loadData(id, exp);
}
Object::~Object() {
    clearList();
    deallocateId(this->idp, this->IdBufferSizeId);
    deallocateInstructions(this->inp, this->instructionBufferSizeId);
    //deallocateMemory(this->inp, this->instructionBufferSizeId, this->idp, this->IdBufferSizeId);
}

//does not free old memory!!!
//only allocates appropriate sized buffer, then copies data supplied in params to new buffer
void Object::loadInstructions(string_view exp)
{
    if(exp.size() < SMALL_EXP)
    {
        this->instructionBufferSizeId = SMALL_EXP;
        this->inp = this->objectMap.getMemMan().small_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < MEDIUM_EXP)
    {
        this->instructionBufferSizeId = MEDIUM_EXP;
        this->inp = this->objectMap.getMemMan().medium_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < LARGE_EXP)
    {
        this->instructionBufferSizeId = LARGE_EXP;
        this->inp = this->objectMap.getMemMan().large_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < VERYLARGE_EXP)
    {
        this->instructionBufferSizeId = VERYLARGE_EXP;
        this->inp = this->objectMap.getMemMan().vlarge_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else
    {
        this->instructionBufferSizeId = exp.size();
        this->inp = malloc(exp.size());
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    memcpy((char*) this->instructions.c_str(), (char*) exp.c_str(), exp.size());
    this->instructions[instructions.size()] = '\0';
}
void Object::loadId(string_view id)
{
    if(id.size() <= SMALL_ID-1)
    {
        this->IdBufferSizeId = SMALL_ID;
        this->idp =this->objectMap.getMemMan().small_id_alloc();
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    else if(id.size() <= LARGE_ID-1)
    {
        this->IdBufferSizeId = LARGE_ID;
        this->idp = this->objectMap.getMemMan().large_id_alloc();
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    else
    {
        this->IdBufferSizeId = id.size();
        this->idp = malloc(id.size());
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    memcpy((char*) this->id.c_str(), id.c_str(), id.size());
    this->id[id.size()] = 0;
}

