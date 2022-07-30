/*
 * PushValAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_ARRPUSHVALACTION_HPP_
#define KEYWORDS_ARRPUSHVALACTION_HPP_

#include "../Keyword.hpp"
#include <cassert>
static inline void pushDoubleArray(KeywordExecutionContext ctx)
{
	AscalExecutor::Operand a = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	ctx.frame().index += Keyword::opcodeSize();
	Object* obj = nullptr;
	ctx.getData(obj);
	ctx.frame().index += Keyword::opcodeSize();
	obj->makeList();
	obj->pushList(a.number());
	
	#ifdef debug
	if(*ctx.runtime().boolsettings["o"])
		std::cout<<"pushing: "<<obj->id<<"["<<obj->getListSize()<<"] = "<<a.constNumber()<<"\n";
	#endif
}

static inline void pushLocalDoubleArray(KeywordExecutionContext ctx)
{
	AscalExecutor::Operand a = ctx.frame().initialOperands.back();
	ctx.frame().initialOperands.pop();
	ctx.frame().index += Keyword::opcodeSize();
	uint64_t stack_index = 0;
	ctx.getData(stack_index);
	ctx.index() += sizeof(stack_index);

	Object* obj = ctx.localMemory()[stack_index].data.obj;
	#ifdef debug
	if(ctx.localMemory()[stack_index].type == StackDataRecord::OWNED || ctx.localMemory()[stack_index].type == StackDataRecord::REFERENCED)
	{
		throw std::string("Assert failed invalid variable type, double cannot be made into list\n");
	}
	if(*ctx.runtime().boolsettings["o"])
		std::cout<<"pushing: "<<obj->id<<"["<<obj->getListSize()<<"] = "<<a.constNumber()<<"\n";
	#endif
	
	obj->makeList();
	obj->pushList(a.number());
	
}
class ArrPushValAction: public OpKeyword {
private:
    ParsedStatementList params;
public:
	ArrPushValAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "arrPush";
	}
	void compile(CompilationContext &ctx) override
	{
		//parse, and compile push object in and then compile params for index
		ctx.src_index += this->keyWord.size();
		const SubStrSV paramsExp = ParsingUtil::getExprInStringSV(ctx.source, ctx.src_index, '(', ')');
		ParsingUtil::ParseStatementList(paramsExp.data, 0, this->params);
		if(this->params.statements.size() != 2)
		{
			throw std::string("Error invalid parameters for arrPush(arr, index)");
		}
		const auto it = ctx.runtime.memory.find(params.statements[0].data);
		if(it != ctx.runtime.memory.end())
		{
			ctx.target.compileParams(params.statements[1].data, ctx.runtime, ctx);
			this->operation = pushDoubleArray;
			ctx.append(this->operation);
			ctx.append((*it).getValue());
		}
		else
		{
			const auto localIt = ctx.localMemory.find(params.statements[0].data);
			if(localIt != ctx.localMemory.end())
			{
				ctx.target.compileParams(params.statements[1].data, ctx.runtime, ctx);
				this->operation = pushLocalDoubleArray;
				ctx.append(this->operation);
				ctx.append((uint64_t) (*localIt).getValue().stack_index);
			}
		}
		ctx.src_index = paramsExp.end+1;
	}
	/*void action(AscalFrame<double>* frame) override
	{
        params.statements.clear();
	    SubStrSV exp = ParsingUtil::getFollowingExprSV(frame->exp, frame->index, keyWord);
        params = ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 2)
	    	throw std::string("arrPush(<object/array>,<value as Ascal expression>)");
	    double valueToPush = runtime.callOnFrame(frame,params.statements[1].data);
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
	    Object *list = runtime.resolveNextExprSafe(frame, vns);
	    list->pushList(valueToPush);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"pushed value "<<params.statements[1].data<<" to list "<<params.statements[0].data<<" at position: "<<(list->getListSize())<<"\n";
	    }
	    frame->index = exp.end;
	}*/
};

#endif /* KEYWORDS_PUSHVALACTION_HPP_ */
