/*
 * Keyword.hpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */
#ifndef KEYWORD_HPP_
#define KEYWORD_HPP_
#include <unordered_map>
#include <map>
#include <string>
#include "Object.hpp"
#include "setting.hpp"
#include "AscalFrame.hpp"
#include "AscalExecutor.hpp"
#include "ParsingUtil.hpp"
#include "CompilationContext.hpp"

//create a wrapper class that has pointers to functions
struct KeywordExecutionContext {
    AscalFrame<double> *&frame_ptr;
	KeywordExecutionContext(AscalFrame<double>*& frame): frame_ptr(frame) {}
	constexpr AscalFrame<double>& frame() const noexcept
	{
		return *frame_ptr;
	}
	constexpr AscalExecutor& runtime() const noexcept
	{
		return frame_ptr->runtime;
	}
	template <typename PLAIN_OLD_OBJECT>
	constexpr void getData(PLAIN_OLD_OBJECT &data, uint32_t byte)
	{
		memcpy(&data, &frame().exp[byte], sizeof(PLAIN_OLD_OBJECT));
	}

};
inline void noop(KeywordExecutionContext);
typedef void (*operationType)(KeywordExecutionContext);
class Keyword {
protected:
	static const std::string MAX;
	std::string keyWord;
	AscalExecutor &runtime;
	operationType operation;
	ParsedStatementList params;
public:
	static inline const std::pair<uint64_t, uint64_t> END_MARKER = std::pair<uint64_t, uint64_t>(std::numeric_limits<uint64_t>::max(), std::numeric_limits<double>::max());
	Keyword(AscalExecutor &runtime);
    operationType getOperation() { return operation; }
	const std::string& getName() { return keyWord; }
	void setName(std::string key) { keyWord = key; }
	virtual void action(AscalFrame<double> *frame) { }
	virtual void compile(CompilationContext &ctx);
	static constexpr uint_fast32_t opcodeSize()
	{
		return  sizeof(void*); //+ sizeof(AscalExecutor::KEYWORD_IDENTIFIER);
	}
	virtual uint8_t getType() { return 'r'; }
	virtual ~Keyword();
	
};

class OpKeyword : public Keyword {
private:
	ParsedStatementList params;
public:
	OpKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual void compile(CompilationContext &ctx) override;
	virtual uint8_t getType() override { return 1; }

};
class StKeyword : public Keyword {
public:
	StKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual uint8_t getType() override { return 0; }
};
class ComplexStKeyword : public Keyword {
public:
	ComplexStKeyword(AscalExecutor &runtime):
	Keyword(runtime){}
	virtual uint8_t getType() override { return 3; }
};

inline void noop(KeywordExecutionContext ctx) { ctx.frame().index += Keyword::opcodeSize(); }
inline void clearStackExcept1(KeywordExecutionContext ctx) { ctx.frame().index += Keyword::opcodeSize(); while(ctx.frame().initialOperands.size() > 1){ctx.frame().initialOperands.pop();} }
inline void clearStack(KeywordExecutionContext ctx) { ctx.frame().index += Keyword::opcodeSize(); ctx.frame().initialOperands.clear(); }

inline void localDoubleVarRead(KeywordExecutionContext ctx)
{
    ctx.frame().index += Keyword::opcodeSize();
    uint64_t index = -1;
    memcpy(&index, &ctx.frame().exp[ctx.frame().index], sizeof(uint64_t));
    ctx.frame().index += sizeof(uint64_t);
    //std::cout<<"Loading data for var id: "<<index<<"\n";
    ctx.frame().initialOperands.push(ctx.frame().localMemory[index].data.number);
    //std::cout<<"value: "<<ctx.frame().localMemory[index].data.number<<"\n";
}
inline void jumpIfFalseAction(KeywordExecutionContext ctx) 
{
	double jmp = -1, boolVal = 0;
	ctx.frame().initialOperands.top(jmp);
	ctx.frame().initialOperands.pop();
	ctx.frame().initialOperands.top(boolVal);
	ctx.frame().initialOperands.pop();
	if(!boolVal && abs(jmp) > Keyword::opcodeSize())
	{
		ctx.frame().index += jmp;
	}
	else
    	ctx.frame().index += Keyword::opcodeSize();
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump if false("<<boolVal<<"): "<<jmp<<'\n';
    }
	#endif
}
inline void jumpIfFalseInlineAction(KeywordExecutionContext ctx) 
{
	double jmp = -1, boolVal = 0;
	ctx.frame().initialOperands.top(boolVal);
	ctx.frame().initialOperands.pop();
	ctx.frame().index += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.frame().index);
	if(!boolVal && abs(jmp) > Keyword::opcodeSize())
	{
		ctx.frame().index += jmp;
	}
	else
    	ctx.frame().index += sizeof(jmp);
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump if false("<<boolVal<<"): "<<jmp<<'\n';
    }
	#endif
}
inline void jumpBackAction(KeywordExecutionContext ctx) 
{
	double jmp = 0;
	ctx.frame().initialOperands.top(jmp);
	ctx.frame().initialOperands.pop();
	ctx.frame().index -= jmp;
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump back: "<<jmp<<'\n';
    }
	#endif
}
inline void jumpForwardInlineAction(KeywordExecutionContext ctx) 
{
	double jmp = 0;
	ctx.frame().index += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.frame().index);
	ctx.frame().index += jmp;
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump forward: "<<jmp<<'\n';
    }
	#endif
}
inline void jumpBackInlineAction(KeywordExecutionContext ctx) 
{
	double jmp = 0;
	ctx.frame().index += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.frame().index);
	ctx.frame().index -= jmp;
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump forward: "<<jmp<<'\n';
    }
	#endif
}
    inline void readAndPushDouble(KeywordExecutionContext ctx)
    {
        double val;
        ctx.frame().index += Keyword::opcodeSize();
        memcpy(&val, &ctx.frame().exp[ctx.frame().index], sizeof(double));
        ctx.frame().initialOperands.push(val);
        ctx.frame().index += sizeof(double);
        #ifdef debug
        if(*ctx.runtime().boolsettings["o"])
            std::cout<<"Pushing double: "<<val<<"\n";
        #endif
    }

#endif /* KEYWORD_HPP_ */
