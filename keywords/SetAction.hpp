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
	double value = -1;
	ctx.frame().initialOperands.top(value);
	ctx.frame().initialOperands.pop();
	Object *obj = nullptr;
    ctx.frame().index += Keyword::opcodeSize();
	memcpy(&obj, &ctx.frame().exp[ctx.frame().index], sizeof(obj));
	obj->setDouble(value);
	ctx.frame().index += sizeof(obj);
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"setting global: "<<obj->getId()<<" to: "<<value<<"\n";
	}
	#endif
}
static inline void setLocalDoubleOp(KeywordExecutionContext ctx)
{
	double value = -1;
	ctx.frame().initialOperands.top(value);
	ctx.frame().initialOperands.pop();
	uint64_t localIndex = 0;
    ctx.frame().index += Keyword::opcodeSize();
	memcpy(&localIndex, &ctx.frame().exp[ctx.frame().index], sizeof(uint64_t));
	ctx.frame().getLocalMemory()[localIndex].data.number = value;
	ctx.frame().index += sizeof(uint64_t);
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
	{
		std::cout<<"setting local: "<<localIndex<<" to: "<<value<<"\n";
	}
	#endif
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
	    uint32_t startIndex = ctx.source.find("=",varName.end)+1;
	    while(ctx.source.size() > startIndex && ctx.source[startIndex] == ' ')
	        startIndex++;
	    SubStrSV subexp = ParsingUtil::getExprInStringSV(ctx.source,startIndex, '{', '}', ';');
		auto localIt = ctx.localMemory.find(varName.data);
		ctx.src_index  = subexp.end;
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
