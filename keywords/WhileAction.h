/*
 * WhileAction.h
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHILEACTION_H_
#define KEYWORDS_WHILEACTION_H_

#include "../Keyword.hpp"
class WhileAction: public ComplexStKeyword {
public:
	WhileAction(AscalExecutor &runtime):
		ComplexStKeyword(runtime)
	{
		this->keyWord = "while";
	}
	void compile(CompilationContext &ctx) override
	{
	    int index = ctx.src_index+5;
	    while(ctx.source[index] == ' ')
	        index++;

	    int startOfBoolExp = index;
	    int startOfCodeBlock = index;
	     

	    while(startOfCodeBlock < ctx.source.size() && ctx.source[startOfCodeBlock] != '{')
	    {
	        startOfCodeBlock++;
	    }

	    index = startOfCodeBlock;
	    const SubStrSV booleanExpression = ParsingUtil::getExprInStringSV(ctx.source, startOfBoolExp, '(', ')', '{');

	    if(booleanExpression.data.size() == 0)
	    {
	        throw std::string("Error no boolean expression provided in while.\n");
	    }
		const uint32_t topOfLoopIndex = ctx.target.getInstructions().size();
		ctx.target.compileParams(booleanExpression.data, runtime, ctx);
		const uint32_t afterBoolExpIndex = ctx.target.getInstructions().size();
		std::cout<<"top of bool exp: "<<topOfLoopIndex<<" after boolExp index: "<<afterBoolExpIndex<<"\n";
		uint32_t startOJumpLenIndex;
		{
			double val = 0;
			
			this->operation = (jumpIfFalseInlineAction);
			ctx.target.append(this->operation);
			startOJumpLenIndex = ctx.target.getInstructions().size();
			ctx.target.append(val);
		}
		const uint32_t afterJumpBoolIndex = reinterpret_cast<std::uintptr_t>(&ctx.target.getInstructions().back()) - reinterpret_cast<std::uintptr_t>(&ctx.target.getInstructions()[0]);
		
	    SubStrSV codeBlock = ParsingUtil::getExprInStringSV(ctx.source, index);
		std::cout<<"\nins len: "<<ctx.target.getInstructions().size()<<"\n";
		ctx.target.compileParams(codeBlock.data, runtime, ctx);
		
		operationType cs = clearStack;
		ctx.target.append(cs);
		const uint32_t afterCodeBlock = ctx.target.getInstructions().size();
		std::cout<<"after bool exp: "<<afterBoolExpIndex<<" after code block: "<<afterCodeBlock<<"\nins len: "<<ctx.target.getInstructions().size()<<"\n";
		{
			uint32_t codeBlockLen_bin = ctx.target.getInstructions().size() - topOfLoopIndex;
			const double val = 8 + codeBlockLen_bin;
			std::cout<<"seting jump back amount: "<<val<<"\n";
			std::cout<<" instructions len: "<<ctx.target.getInstructions().size()<<"\n";
			
			this->operation = jumpBackInlineAction;
			ctx.target.append(this->operation);
			ctx.target.append(val);
		}
		{
			std::cout<<" instructions len: "<<ctx.target.getInstructions().size()<<"\n";
			const double val = ctx.target.getInstructions().size() - startOJumpLenIndex;
			std::cout<<"seting jump forward amount: "<<val<<"\n";
			memcpy(&ctx.target.getInstructions()[startOJumpLenIndex], &val, sizeof(val));
		}
	   	index = codeBlock.end + startOfCodeBlock-2;
	    ctx.src_index = 0;
	    while(ctx.source[index] == ';' || ctx.source[index] == ' ' || ctx.source[index] == '}')
	        index++;
	    ctx.src_index = codeBlock.end;//(index-2<frame->exp.size()?index-2:frame->exp.size());
	}
};

#endif /* KEYWORDS_WHILEACTION_H_ */
