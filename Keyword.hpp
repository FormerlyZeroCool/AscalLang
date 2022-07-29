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
namespace asc {
inline double abs(double value) noexcept
{
	return value < 0 ? value * -1 : value;
}
}
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
	constexpr auto& index() noexcept
	{
		return this->frame().index;
	}
	template <typename PLAIN_OLD_OBJECT>
	constexpr void getData(PLAIN_OLD_OBJECT &data, uint32_t byte)
	{
		memcpy(&data, &frame().exp[byte], sizeof(PLAIN_OLD_OBJECT));
	}
	template <typename PLAIN_OLD_OBJECT>
	constexpr void getData(PLAIN_OLD_OBJECT &data)
	{
		memcpy(&data, &frame().exp[index()], sizeof(PLAIN_OLD_OBJECT));
	}
	constexpr auto& localMemory()
	{
		return this->frame().localMemory;
	}
	constexpr auto& localMemory(uint32_t index)
	{
		return this->frame().localMemory[index].data;
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

inline void noop(KeywordExecutionContext ctx) { ctx.index() += Keyword::opcodeSize(); }
inline void clearStackExcept1(KeywordExecutionContext ctx) { ctx.index() += Keyword::opcodeSize(); while(ctx.frame().initialOperands.size() > 1){ctx.frame().initialOperands.pop();} }
inline void clearStack(KeywordExecutionContext ctx) { ctx.index() += Keyword::opcodeSize(); ctx.frame().initialOperands.clear(); }

inline void localDoubleVarRead(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t index = -1;
    memcpy(&index, &ctx.frame().exp[ctx.index()], sizeof(uint64_t));
    ctx.index() += sizeof(uint64_t);
    //std::cout<<"Loading data for var id: "<<index<<"\n";
    ctx.frame().initialOperands.push(ctx.frame().localMemory[index].data.number);
    //std::cout<<"value: "<<ctx.frame().localMemory[index].data.number()<<"\n";
}
inline void jumpIfFalseAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand jmp(-1.0), boolVal(0.0);
	ctx.frame().initialOperands.top(jmp);
	ctx.frame().initialOperands.pop();
	ctx.frame().initialOperands.top(boolVal);
	ctx.frame().initialOperands.pop();
	if(!boolVal.number() && asc::abs(jmp.number()) > Keyword::opcodeSize())
	{
		ctx.index() += jmp.number();
	}
	else
    	ctx.index() += Keyword::opcodeSize();
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump if false("<<boolVal.number()<<"): "<<jmp.number()<<'\n';
    }
	#endif
}
inline void jumpIfFalseInlineAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand jmp(-1.0), boolVal(0.0);
	ctx.frame().initialOperands.top(boolVal);
	ctx.frame().initialOperands.pop();
	ctx.index() += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.index());
	if(!boolVal.number() && asc::abs(jmp.number()) > Keyword::opcodeSize())
	{
		ctx.index() += jmp.number();
	}
	else
    	ctx.index() += sizeof(jmp.number());
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump if false("<<boolVal.number()<<"): "<<jmp.number()<<'\n';
    }
	#endif
}
inline void jumpBackAction(KeywordExecutionContext ctx) 
{
	AscalExecutor::Operand jmp(0.0);
	ctx.frame().initialOperands.top(jmp);
	ctx.frame().initialOperands.pop();
	ctx.index() -= jmp.number();
	#ifdef debug
    if(*ctx.runtime().boolsettings["o"])
    {
    	std::cout<<"jump back: "<<jmp.number()<<'\n';
    }
	#endif
}
inline void jumpForwardInlineAction(KeywordExecutionContext ctx) 
{
	double jmp = (0.0);
	ctx.index() += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.index());
	ctx.index() += jmp;
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
	ctx.index() += Keyword::opcodeSize();
	ctx.getData(jmp, ctx.index());
	ctx.index() -= jmp;
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
    ctx.index() += Keyword::opcodeSize();
	ctx.getData(val, ctx.index());
    ctx.frame().initialOperands.push(val);
    ctx.index() += sizeof(double);
    #ifdef debug
    if(*ctx.runtime().boolsettings["o"])
        std::cout<<"Pushing double: "<<val<<"\n";
    #endif
}
inline void readAndPushObject(KeywordExecutionContext ctx)
{
    Object* val;
    ctx.index() += Keyword::opcodeSize();
	ctx.getData(val, ctx.index());
    ctx.frame().initialOperands.push(val);
    ctx.index() += sizeof(Object*);
    #ifdef debug
    if(*ctx.runtime().boolsettings["o"])
        std::cout<<"Pushing Global Object to data stack: "<<val->getId()<<"\n";
    #endif
}
inline void readAndPushLocalObject(KeywordExecutionContext ctx)
{
    uint64_t val;
    ctx.index() += Keyword::opcodeSize();
	ctx.getData(val);
    ctx.frame().initialOperands.push(ctx.localMemory()[val].data.obj);
    ctx.index() += sizeof(val);
	
    #ifdef debug
    if(*ctx.runtime().boolsettings["o"])
        std::cout<<"Pushing local Object to data stack: "<<ctx.localMemory()[val].data.obj->getId()<<"\n";
    #endif
}
inline Object& popObject(KeywordExecutionContext ctx)
{
    AscalExecutor::Operand val = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	Object* obj = nullptr;
	if(val.integer() > 256)
	{
		obj = val.object();
	}
	else
	{
		obj = ctx.frame().getLocalMemory()[val.integer()].data.obj;
	}
	return *obj;
}
inline AscalExecutor::Operand callSub(KeywordExecutionContext ctx)
{
	uint64_t expressionSize = 0;
	ctx.getData(expressionSize);
	ctx.index() += sizeof(expressionSize);
	double index;
	{
		AscalFrame<double> subexp(ctx.frame());
		subexp.index = 0;
		ctx.frame().initialOperands.resetStart();
		subexp.exp = ctx.frame().exp.substr(ctx.index(), expressionSize);
		index = ctx.runtime().calculateExpression(&subexp);
		subexp.localMemory.resetStart();
		ctx.index() += (expressionSize);
	}
	return index;
}
inline void readAndPushFromGlobalList(KeywordExecutionContext ctx)
{
	const Object* obj = &popObject(ctx);
    ctx.index() += Keyword::opcodeSize();
	const double index = callSub(ctx).number();


	if(obj->isDoubleList())
	{
		ctx.frame().initialOperands.push(obj->getDoubleAtIndex(index));
    	#ifdef debug
    	if(*ctx.runtime().boolsettings["o"])
    	    std::cout<<"Pushing number from list: "<<ctx.frame().initialOperands.back().number()<<"\n";
    	#endif
	}
	else if(obj->isObjList())
	{
		ctx.frame().initialOperands.push(&obj->getObjectAtIndex(index));
    	#ifdef debug
    	if(*ctx.runtime().boolsettings["o"])
    	    std::cout<<"Pushing Object from list: "<<obj->id<<"\n";
    	#endif
	}
}
inline void compileLoop(CompilationContext &ctx, string_view booleanExpression, string_view codeBlock)
	{
		operationType operation = nullptr;
		const uint32_t topOfLoopIndex = ctx.target.getInstructions().size();
		ctx.target.compileParams(booleanExpression, ctx.runtime, ctx);
		uint32_t startOJumpLenIndex;
		{
			double val = 0;
			
			operation = (jumpIfFalseInlineAction);
			ctx.target.append(operation);
			startOJumpLenIndex = ctx.target.getInstructions().size();
			ctx.target.append(val);
		}
		ctx.target.compileParams(codeBlock, ctx.runtime, ctx);
		
		operation = clearStack;
		ctx.target.append(operation);
		{
			uint32_t codeBlockLen_bin = ctx.target.getInstructions().size() - topOfLoopIndex;
			const double val = sizeof(void*) + codeBlockLen_bin;
			
			operation = jumpBackInlineAction;
			ctx.target.append(operation);
			ctx.target.append(val);
		}
		{
			const double val = ctx.target.getInstructions().size() - startOJumpLenIndex;
			memcpy(&ctx.target.getInstructions()[startOJumpLenIndex], &val, sizeof(val));
		}
	}
inline void compileLoop(CompilationContext &ctx, string_view booleanExpression, string_view codeBlock, string_view codeBlock2, string_view initialization)
{
	ctx.target.compileParams(initialization, ctx.runtime, ctx);
	operationType operation = nullptr;
	const uint32_t topOfLoopIndex = ctx.target.getInstructions().size();
	ctx.target.compileParams(booleanExpression, ctx.runtime, ctx);
	uint32_t startOJumpLenIndex;
	{
		double val = 0;
		
		operation = (jumpIfFalseInlineAction);
		ctx.target.append(operation);
		startOJumpLenIndex = ctx.target.getInstructions().size();
		ctx.target.append(val);
	}
	ctx.target.compileParams(codeBlock, ctx.runtime, ctx);
	ctx.target.compileParams(codeBlock2, ctx.runtime, ctx);
	
	operation = clearStack;
	ctx.target.append(operation);
	{
		uint32_t codeBlockLen_bin = ctx.target.getInstructions().size() - topOfLoopIndex;
		const double val = sizeof(void*) + codeBlockLen_bin;
		
		operation = jumpBackInlineAction;
		ctx.target.append(operation);
		ctx.target.append(val);
	}
	{
		const double val = ctx.target.getInstructions().size() - startOJumpLenIndex;
		memcpy(&ctx.target.getInstructions()[startOJumpLenIndex], &val, sizeof(val));
	}
}

namespace Global {
static inline void makeArray(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
}
static inline void makeDouble(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t size = -1;
    memcpy(&size, &ctx.frame().exp[ctx.index()], sizeof(uint64_t));
    ctx.index() += sizeof(uint64_t);
    const auto nameIndex = ctx.index();
    ctx.index() += size;
    Object *obj = ctx.runtime().memMan.constructObj(string_view(&ctx.frame().exp[nameIndex], size), 
                        string_view("", 0));
    double value = -1;
    memcpy(&value, &ctx.frame().exp[ctx.index()], sizeof(double));
    ctx.index() += sizeof(double);
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
    ctx.index() += Keyword::opcodeSize();
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->initialOperands;
    
    AscalExecutor::Operand &var = ctx.frame().initialOperands.back();
    ctx.frame().getLocalMemory().push(StackDataRecord(StackDataRecord::DOUBLE, var.number()));
    localMem.pop();
	#ifdef debug
    std::cout<<"parameter created, double: "<<var.number()<<"\n"<<"Var stack id: "<<ctx.frame().getLocalMemory().size();
    #endif
}
static inline void makeObjectParameter(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    StackDataRecord value;
    auto &localMem = (ctx.runtime().frameStack)[ctx.runtime().frameStack.size() - ((ctx.runtime().frameStack.size() > 1) << 1)]->localMemory;
    localMem.top(value);
    localMem.pop();
    //std::cout<<"paramdouble: "<<value<<"\n";
    ctx.frame().getLocalMemory().push(value);
}
static inline void makeFunction(KeywordExecutionContext ctx)
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t nameSize = -1;
    memcpy(&nameSize, &ctx.frame().exp[ctx.index()], sizeof(uint64_t));
    ctx.index() += sizeof(uint64_t);
    const auto nameIndex = ctx.index();
    ctx.index() += nameSize;
    uint64_t bodySize = -1;
    memcpy(&bodySize, &ctx.frame().exp[ctx.index()], sizeof(uint64_t));
    ctx.index() += sizeof(uint64_t);
    Object obj(ctx.runtime().memMan, string_view(&ctx.frame().exp[nameIndex], nameSize), 
                        string_view(&ctx.frame().exp[ctx.index()], bodySize));
    
    ctx.runtime().loadUserDefinedFn(obj, ctx.runtime().memory);
    ctx.index() += bodySize;
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
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"Returning value: "<<ctx.frame().initialOperands.back().number()<<"\n";
	}
	#endif
    //    std::cout<<data<<"\n";
}
};


#endif /* KEYWORD_HPP_ */
