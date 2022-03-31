/*
 * Ascal.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "Ascal.hpp"
	double Ascal::execExpression(string_view exp)
	{
		FunctionFrame<double> *current = nullptr;
		double value = execExpression(exp, current);
		runtime.fFramePool.destroy(current);
		return value;
	}
	double Ascal::execExpression(string_view exp, FunctionFrame<double>* &current)
	{
		current = runtime.fFramePool.construct(runtime, runtime.memMan);
        current->setIsDynamicAllocation(false);
		current->exp = exp;
	try{
		return runtime.calcWithOptions(current);
	}
    catch(std::string &exception)
    {
        std::cerr<<"Function call stack trace.\n";
        std::cerr<<exception<<std::endl;
        std::cerr<<"End of function call stack trace.\n";
        std::cerr<<"Failed to exec: "<<exp<<std::endl;
        return 0;
    }
	}
