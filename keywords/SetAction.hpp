/*
 * SetAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_SETACTION_HPP_
#define KEYWORDS_SETACTION_HPP_

#include "../Keyword.hpp"
static inline void setGlobalDoubleOp(KeywordExecutionContext ctx)
{
	AscalExecutor::Operand value(0.0);
	ctx.frame().initialOperands.top(value);
	ctx.frame().initialOperands.pop();
	Object *obj = nullptr;
    ctx.frame().index += Keyword::opcodeSize();
	ctx.getData(obj, ctx.frame().index);
	obj->setDouble(value.number());
	ctx.frame().index += sizeof(obj);
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"setting global: "<<obj->getId()<<" to: "<<value.number()<<"\n";
	}
	#endif
}
static inline void setLocalDoubleOp(KeywordExecutionContext ctx)
{
	AscalExecutor::Operand value(0.0);
	ctx.frame().initialOperands.top(value);
	ctx.frame().initialOperands.pop();
	uint64_t localIndex = 0;
    ctx.frame().index += Keyword::opcodeSize();
	ctx.getData(localIndex, ctx.frame().index);
	ctx.frame().getLocalMemory()[localIndex].data.number = value.number();
	ctx.frame().index += sizeof(uint64_t);
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"setting local: "<<localIndex<<" to: "<<value.number()<<"\n";
	}
	#endif
}
static inline void setList(KeywordExecutionContext ctx, Object* obj, AscalExecutor::Operand value, const AscalExecutor::Operand index_as_double)
{
	if(obj->isDoubleList())
	{
		obj->setDoubleAtIndex(index_as_double.constNumber(), value.constNumber());
	}
	else if(obj->isObjList())
	{
		obj->setList(*value.object(), index_as_double.constNumber());
	}
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
		std::cout<<"setting: "<<obj->id<<"["<<index_as_double.constNumber()<<"] = "<<value.constNumber()<<"\n";
	#endif
}
static inline void setGlobalArrayOp(KeywordExecutionContext ctx)
{
	const AscalExecutor::Operand value = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	const AscalExecutor::Operand index_as_double = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	Object *obj = nullptr;
	ctx.index() += Keyword::opcodeSize();
	ctx.getData(obj);
	ctx.index() += Keyword::opcodeSize();
	setList(ctx, obj, value, index_as_double);
}
static inline void setLocalArrayOp(KeywordExecutionContext ctx)
{
	const AscalExecutor::Operand value = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	const AscalExecutor::Operand index_as_double = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	uint64_t stack_index = 0;
	ctx.index() += Keyword::opcodeSize();
	ctx.getData(stack_index);
	ctx.index() += sizeof(stack_index);

	Object *obj = ctx.localMemory(stack_index).obj;
	setList(ctx, obj, value, index_as_double);
}
class SetAction: public StKeyword {
public:
	SetAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "set";
	}
	void compile(CompilationContext &ctx) override
	{
		static const std::string keyWord = "set";
		uint32_t index = ctx.src_index+keyWord.size();
	    SubStrSV varName = ParsingUtil::getVarNameSV(ctx.source, index);
		++ctx;
		++ctx;
		std::cout<<"current: "<<ctx.current().source<<"\n";
		SubStrSV maybeArraySubScript = getExpr(ctx, ctx.findTokenIndexAtLevel(string_view("=", 1), 0, '[', ']'), '[', ']');
	    uint32_t startIndex = ctx.source.find("=",varName.end)+1;
	    SubStrSV subexp = ParsingUtil::getExprInStringSV(ctx.source, startIndex, '{', '}', ';');
		auto localIt = ctx.localMemory.find(varName.data);
		ctx.src_index  = subexp.end;
			std::cout<<"full array subscript substr: "<<maybeArraySubScript.data<<"\n";
		if(ParsingUtil::firstChar(maybeArraySubScript.data, '['))
		{
			const string_view arraySubScript = maybeArraySubScript.data.substr(1, maybeArraySubScript.data.size() - 2);
			std::cout<<"trimmed subscript: "<<arraySubScript<<"\n";
			if(localIt != ctx.localMemory.end())
			{
				ctx.target.compileParams(arraySubScript, ctx.runtime, ctx);
        		ctx.target.compileParams(subexp.data, ctx.runtime, ctx);

				const CompilationContext::LocalRecord localRec = (*localIt).getValue();
				this->operation = setLocalArrayOp;
				ctx.target.append(this->operation);
				ctx.target.append((uint64_t) (localRec.stack_index));
			}
			else
			{
        		auto it = ctx.runtime.memory.find(varName.data);
        		if(it != ctx.runtime.memory.end())
        		{
					ctx.target.compileParams(arraySubScript, ctx.runtime, ctx);
        		    ctx.target.compileParams(subexp.data, ctx.runtime, ctx);
        		    this->operation = setGlobalArrayOp;
        		    ctx.target.append(this->operation);
        		    ctx.target.append((*it).getValue());
        		}

			}
		
		}
		else
		{
			if(localIt != ctx.localMemory.end())
			{
        		ctx.target.compileParams(subexp.data, ctx.runtime, ctx);
				const CompilationContext::LocalRecord localRec = (*localIt).getValue();
				this->operation = setLocalDoubleOp;
				ctx.target.append(this->operation);
				ctx.target.append((uint64_t) (localRec.stack_index));
			}
			else
			{
        		auto it = ctx.runtime.memory.find(varName.data);
        		if(it != ctx.runtime.memory.end())
        		{
        		    ctx.target.compileParams(subexp.data, ctx.runtime, ctx);
        		    this->operation = setGlobalDoubleOp;
        		    ctx.target.append(this->operation);
        		    ctx.target.append((*it).getValue());
        		}

			}
		}

	}
	void action(AscalFrame<double>* frame) override
	{
		static const std::string keyWord = "set";
		uint32_t index = frame->index+keyWord.size();
	    SubStrSV varName = ParsingUtil::getVarNameSV(frame->exp, index);
	    uint32_t startIndex = frame->exp.find("=",varName.end)+1;
	    while(frame->exp.size() > startIndex && frame->exp[startIndex] == ' ')
	        startIndex++;
	    string_view subexp;
	    {
	    	uint32_t sibk = startIndex;
	    	subexp = ParsingUtil::getExprInStringSV(frame->exp,startIndex, '{', '}', ';');
	    	startIndex = sibk;
	    }
	    expressionResolution res = runtime.resolveNextObjectExpression(frame, varName);
        Object *obj = res.data;
        if(!obj && res.parent)
        {
            frame->initialOperands.pop();
            if(!ParsingUtil::isDouble(subexp))
            {
                //res.parent->setDoubleAtIndex(res.listIndex, runtime.callOnFrame(frame,subexp));
            }
            else
            {
                char tmp = subexp[subexp.size()];
                subexp[subexp.size()] = 0;
                res.parent->setDoubleAtIndex(res.listIndex, atof(&subexp[0]));
                subexp[subexp.size()] = tmp;
            }
        }
        else if(obj){
	    if(!ParsingUtil::isObj(subexp))
	    {
	    	if(!ParsingUtil::isDouble(subexp))
	    	{
	    		//obj->setDouble(runtime.callOnFrame(frame,subexp));
	    	}
	    	else
	    	{
	    		char tmp = subexp[subexp.size()];
	    		subexp[subexp.size()] = 0;
	    		obj->setDouble(atof(&subexp[0]));
	    		subexp[subexp.size()] = tmp;
	    	}
    	    //if(*runtime.boolsettings["o"])
    	    //{
    	    //	std::cout<<"set "<<varName.data<<" = "<< obj->getDouble()<<"\n";
    	    //}
	    }
	    else 
	    {
	    	SubStrSV rightHandObjectLookup = ParsingUtil::getVarNameSV(frame->exp, startIndex);
	    	Object *rightObj = runtime.resolveNextExprSafe(frame, rightHandObjectLookup);
	    	obj->copyExceptID(*rightObj);
            
		    if(*runtime.boolsettings["o"])
		    {
		    	std::cout<<"set "<<varName.data<<" = "<<obj->toString()<<"\n";
		    }
	    }
        
	}
  }
};

#endif /* KEYWORDS_SETACTION_HPP_ */
