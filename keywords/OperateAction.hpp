/*
 * PauseAction.h
 *
 *  Created on: Apr 22, 2022
 *      Author: andrewrubinstein
 */

#pragma once

#include "../Keyword.hpp"
static Calculator<double> calculator;
inline void operateAction(KeywordExecutionContext ctx) 
{
    double a = -1, b = -1;
    ctx.frame().initialOperands.top(a);
    ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.top(b);
    ctx.frame().initialOperands.pop();
    ctx.frame().initialOperands.push(calculator.calc(ctx.frame().exp[ctx.frame().index + Keyword::opcodeSize()], b, a));
    #ifdef debug
    if(*ctx.runtime.boolsettings["o"])
        std::cout<<b<<ctx.frame().exp[ctx.frame().index + Keyword::opcodeSize()]<<a<<"\n";
    #endif
    ctx.frame().index += Keyword::opcodeSize() + 1;
}
class OperateAction: public OpKeyword {
public:
	OperateAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "operatorCall";
        this->operation = operateAction;
	}
};
