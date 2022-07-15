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
	    SubStrSV codeBlock = ParsingUtil::getExprInStringSV(ctx.source, index);
		compileLoop(ctx, booleanExpression.data, codeBlock.data);
	   	index = codeBlock.end + startOfCodeBlock-2;
	    ctx.src_index = 0;
	    while(ctx.source[index] == ';' || ctx.source[index] == ' ' || ctx.source[index] == '}')
	        index++;
	    ctx.src_index = codeBlock.end;//(index-2<frame->exp.size()?index-2:frame->exp.size());
	}
};

#endif /* KEYWORDS_WHILEACTION_H_ */
