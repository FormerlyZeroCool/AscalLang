/*
 * PrintCommand.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTCOMMAND_HPP_
#define KEYWORDS_PRINTCOMMAND_HPP_

#include "../Keyword.hpp"
#include "PrintVarAction.hpp"
#include "PrintCalculation.hpp"
class PrintCommand: public StKeyword {
private:
	PrintVarAction pvar;
	PrintCalculation pcvar;
	void printAllFunctions()
	{
	    std::cout<<"All Functions in Memory:"<<std::endl;
	    for(auto &value : runtime.memory)
	    {
	        std::cout<<std::endl<<"Function Name: "<<value.getId()<<"\nFunction Expression: "<<value.toString()<<std::endl;
	    }
	    std::cout<<std::endl<<"End of All Functions in Memory."<<std::endl;
	}
	void printAllUDF(AscalFrame<double> *frame)
	{
	    std::cout<<"User Defined Functions:"<<std::endl;
	    for(Object & data:runtime.userDefinedFunctions)
	    {
	        std::cout<<std::endl<<"Function Name: "<<data.getId()<<std::endl<<"Function Expression: "<<runtime.getObject(frame, data.getId()).toString()<<std::endl;

	    }
	    std::cout<<std::endl<<"End of User Defined Functions."<<std::endl;
	}
	void printAllSDF()
	{
	    std::cout<<"System Defined Functions:"<<std::endl;
	    for(Object & data:runtime.systemDefinedFunctions)
	    {
	        std::cout<<std::endl<<"Function Name: "<<data.getId()<<std::endl<<"Function Expression: "<<data.instructionsToFormattedString()<<std::endl;
	    }
	    std::cout<<std::endl<<"End of System Defined Functions."<<std::endl;
	}
public:
	PrintCommand(AscalExecutor &runtime):
	StKeyword(runtime), pvar(runtime), pcvar(runtime)
	{
		this->keyWord = "printc";
	}
	void action(AscalFrame<double>* frame) override
	{
	    std::cout<<"\n";
	    int start = frame->exp.find("print",frame->index)+6;
	            if(ParsingUtil::cmpstr(frame->exp.substr(start,3),std::string("all")))
	            {
	                printAllFunctions();
	            }
	            else if(ParsingUtil::cmpstr(frame->exp.substr(start,3),std::string("udf")))
	            {
	                printAllUDF(frame);
	            }
	            else if(ParsingUtil::cmpstr(frame->exp.substr(start,3),std::string("sdf")))
	            {
	                printAllSDF();
	            }
	            else if(ParsingUtil::cmpstr(frame->exp.substr(start,3),std::string("var")))
	            {
	            	//print data in variable as object
	            	pvar.action(frame);
	            }
	            else
	            {
	            	//Print result of calculating expression following print statement
	                pcvar.action(frame);
	            }
	}
};

#endif /* KEYWORDS_PRINTCOMMAND_HPP_ */
