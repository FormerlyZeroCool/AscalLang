/*
 * PauseAction.h
 *
 *  Created on: Apr 25, 2022
 *      Author: andrewrubinstein
 */

#pragma once

#include "../Keyword.hpp"
class OperatePushDoubleAction: public OpKeyword {
public:
	OperatePushDoubleAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "pushDouble";
	}
	void action(AscalFrame<double>* frame) override
	{
        double val;
        memcpy(&val, &frame->exp[frame->index + this->opcodeSize()], sizeof(double));
        frame->initialOperands.push(val);
	    frame->index += this->opcodeSize() + sizeof(double);
	}
};

