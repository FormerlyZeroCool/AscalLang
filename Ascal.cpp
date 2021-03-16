/*
 * Ascal.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "Ascal.hpp"
	double Ascal::execExpression(std::string &&exp)
	{
		return execExpression(exp);
	}
	double Ascal::execExpression(std::string &exp)
	{
		AscalFrame<double> *frame = (AscalFrame<double> *)new FunctionFrame<double>(runtime, runtime.memMan);
		frame->exp = exp;
	try{
		return runtime.calcWithOptions(frame);
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
