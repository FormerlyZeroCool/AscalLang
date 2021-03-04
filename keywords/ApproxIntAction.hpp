/*
 * ApproxIntAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_APPROXINTACTION_HPP_
#define KEYWORDS_APPROXINTACTION_HPP_

#include "../Keyword.hpp"
class ApproxIntAction: public OpKeyword {
public:
	ApproxIntAction(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "approxInt";
	}
	void action(AscalFrame<double>* frame) override
	{
	    const int plotKeyWordIndex = frame->exp.find("approxInt",frame->index);
	    const int endOfFun = frame->exp.find(",",frame->index);
	    const int endOfDomain = frame->exp.find(",",endOfFun+1);
	    std::vector<double> sumArea;
	    sumArea.push_back(0);
	    int index = plotKeyWordIndex+9;
	    while(frame->exp[index] && frame->exp[index] == ' ')
	    {
	        index++;
	    }
	    std::vector<std::string> functions;
	    int trailer = index;
	    {
	        while(frame->exp[index] && frame->exp[index] != ',')
	        {
	            if(frame->exp[index] == '|')
	            {
	                functions.push_back(frame->exp.substr(trailer,index-(trailer)).str());
	                sumArea.push_back(0);
	                trailer = index+1;
	            }
	                index++;
	        }
	    }
	    functions.push_back(frame->exp.substr(trailer,endOfFun - (trailer)).str());
	    index = endOfFun + 1;
	    //getNextDouble updates index to be the character at the end of the double
	    const double xMin = ParsingUtil::getNextDouble(frame->exp,index);
	    index += 2;
	    const double xMax = ParsingUtil::getNextDouble(frame->exp,index);
	    index = endOfDomain + 1;
	    const int tableWidth = ParsingUtil::getNextDouble(frame->exp,index);
	    index += 2;
	    const char methodIdentifier = frame->exp[index];
	    if(methodIdentifier != 'l' && methodIdentifier != 't' && methodIdentifier != 'r')
	    	index -=2;
	    const double xStepSize = (xMax-xMin)/(tableWidth>0?tableWidth:1);
	    double dx = (xMax-xMin)/tableWidth;
	    double xi;
	    double thisIndex = 0;
	    double lastIndex = 0;
	    std::stringstream exp;
	    for(int j = 0;j<functions.size();j++)
	    {
	        std::string function = functions[j];
	        for(int i = 0;i<tableWidth;i++)
	        {
	            xi = xMin+dx*(i+1);
	            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(runtime, runtime.memMan);
	            exp << function << '(' << ParsingUtil::to_string(xi) << ')';
	            calledFunction->exp = exp.str();
	            exp.str(std::string());
	            thisIndex = runtime.calculateExpression(calledFunction);//outPuts.get(i,j)*dx;

	            sumArea[j] +=
	                    //Trapezoidal this is the defaulr
	                    ((methodIdentifier != 'r' && methodIdentifier != 'l') * ((thisIndex + lastIndex) * dx / 2)) +
	                    //left Riemann sum
	                    ((methodIdentifier == 'l') * dx*lastIndex) +
	                    //right Riemann sum
	                    ((methodIdentifier == 'r') * dx*thisIndex);

	            lastIndex = thisIndex;
	        }
	    }
	    std::string calculationType;

	    if(methodIdentifier == 'r')
	    {
	        calculationType = "right handed";
	    }
	    else if(methodIdentifier == 'l')
	    {
	        calculationType = "left handed";
	    }
	    else
	    {
	        calculationType = "trapezoidal";
	    }
	    std::cout<<"\n"<<frame->exp<<"\n";
	    std::cout<<"domain:"<<xMin<<" to "<<xMax<<" with a step size in the x of: "<<xStepSize<<"\n";
	    for(int i =0; i<functions.size();i++)
	    {
	        std::cout<<"Function: "<<functions[i]<<", function defined as: "<<runtime.memory.find(functions[i]).instructionsToFormattedString();
	        std::cout<<"Area Under Curve calculated with "<<calculationType<<" Reimann sum using "<<tableWidth<<" partitions: "<<sumArea[i]<<"\n\n";
	    }
	    frame->index = index + 1;
	}
};

#endif /* KEYWORDS_APPROXINTACTION_HPP_ */
