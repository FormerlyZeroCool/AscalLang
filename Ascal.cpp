/*
 * Ascal.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "Ascal.hpp"
	double Ascal::execExpression(string_view exp)
	{
		AscalFrame<double> *current = nullptr;
		double value = execExpression(exp, current);
		runtime.framePool.destroy(current);
		return value;
	}
	double Ascal::execExpression(string_view exp, AscalFrame<double>*& current)
	{
		Object compiledCode(runtime.memMan, string_view("", 0), exp);
		compiledCode.compileInstructions(runtime);
		current = runtime.framePool.construct(runtime);
		current->exp = compiledCode.getInstructions();
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
