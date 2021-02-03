/*
 * PlotAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PLOTACTION_HPP_
#define KEYWORDS_PLOTACTION_HPP_

#include "../Keyword.hpp"
#include "../Vect2D.hpp"
class PlotAction: public Keyword {
public:
	PlotAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "plot";
	}

	std::string action(AscalFrame<double>* frame) override
	{
	    const int plotKeyWordIndex = frame->exp.find("plot",frame->index);
	    const int endOfFun = frame->exp.find(",", plotKeyWordIndex);
	    const int endOfDomain = frame->exp.find(",",endOfFun+1);
	    std::vector<double> sumArea;
	    sumArea.push_back(0);
	    int index = plotKeyWordIndex+4;
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
	                functions.push_back(frame->exp.substr(trailer,index-(trailer)));
	                sumArea.push_back(0);
	                trailer = index+1;
	            }
	                index++;
	        }
	    }
	    functions.push_back(frame->exp.substr(trailer,endOfFun - (trailer)));
	    index = endOfFun+1;
	    const double xMin = ParsingUtil::getNextDoubleS(frame->exp,index);
	    index+=2;
	    const double xMax = ParsingUtil::getNextDoubleS(frame->exp,index);
	    index = endOfDomain+1;
	    const double yMin = ParsingUtil::getNextDoubleS(frame->exp,index);
	    index +=2;
	    const double yMax = ParsingUtil::getNextDoubleS(frame->exp,index);
	    index +=2;
	    const double xStepSize = ParsingUtil::getNextDoubleS(frame->exp,index);
	    index += 2;
	    const double yStepSize = ParsingUtil::getNextDoubleS(frame->exp,index);
	    int tableWidth = (xMax-xMin)/(xStepSize>0?xStepSize:1);
	    int tableHeight = (yMax-yMin)/(yStepSize>0?yStepSize:1);
	    double dx = (xMax-xMin)/tableWidth;
	    double dy = yStepSize>0?yStepSize:1;
	    double xi;
	    Vect2D<double> outPuts(tableWidth,functions.size()-1);
	    std::stringstream exp;
	    for(int j = 0;j<functions.size();j++)
	    {
	        std::string function = functions[j];
	        if(*(*boolsettings)["o"])
	        	std::cout<<"\nProcessing: "<<function<<"\n";
	        for(int i = 0;i<tableWidth;i++)
	        {
	            xi = xMin+dx*(i);
	            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
	            exp << function << '(' << ParsingUtil::to_string(xi) << ')';
	            calledFunction->exp = exp.str();
	            exp.str(std::string());
	            outPuts.push_back(
	                    runtime->calculateExpression(calledFunction));
	            sumArea[j] += outPuts.get(i,j)*dx;
	        }
	    }

	    double yClosestToZero = yMax;
	    for(int y = tableHeight+1;y>=0;y--)
	    {
	        if(std::abs(yMin+dy*y)<yClosestToZero)
	            yClosestToZero = yMin+dy*y;
	    }
	    double xClosestToZero = xMax;
	    std::cout<<"\n";
	    for(int i = 0;i<=tableWidth;i++)
	    {
	        xi = (dx*i+xMin);

	        if(std::abs(xi) < std::abs(xClosestToZero))
	            xClosestToZero = xi;
	        if(i%5 == 0)
	            std::cout<<std::to_string(xi).substr(0,4);
	        else if((i-1)%5 == 0 || (i-2)%5 == 0|| (i-3)%5 == 0){}
	        else
	            std::cout<<" ";
	    }
	    std::cout<<" "<<"\n";
	    for(int x = 0;x<tableWidth;x++)
	    {
	        std::cout<<"|";
	    }
	    std::cout<<' '<<xMax<<'\n';
	    char symbols[10] = {'*','#','%','@','$','&','=','+','-','!'};
	    for(double y = tableHeight+1;y>=0;y--)
	    {

	        for(int x = 0;x<tableWidth;x++)
	        {
	            bool drawNonFn = true;
	            for(int z = outPuts.getHeight(); z>=0;z--)
	            {
	                const double roundedFunctionOutput = (std::round((outPuts.get(x,z) - yMin) / dy) * dy) + yMin;
	                bool drawFnValue = true;
	                //this inner loop is to check if another symbol would have been printed for this point
	                //runs n^2 where n is basically limited to between 1 and 10, but could def be optimized, and it's run a lot at once
	                for(int a =0;drawFnValue && a < z;a++)
	                {
	                    drawFnValue = (roundedFunctionOutput != (std::round((outPuts.get(x,a) - yMin) / dy) * dy) + yMin);
	                }
	                if(drawFnValue && y*dy+yMin == roundedFunctionOutput)
	                {
	                    std::cout<<(symbols[z%10]);
	                    drawNonFn = false;
	                }
	            }
	            if(drawNonFn)
	            {
	                if(yMin+dy*y == yClosestToZero)
	                    std::cout<<"-";
	                else if(xMin+dx*x == xClosestToZero)
	                    std::cout<<"|";
	                else
	                    std::cout<<" ";
	            }
	        }
	        std::cout<<" "<<std::round((y*dy+yMin)*10000)/10000<<"\n";
	    }

	    for(int x = 0;x<tableWidth;x++)
	    {
	        std::cout<<"|";
	    }
	    std::cout<<"\n"<<frame->exp<<"\n";
	    std::cout<<"domain:"<<xMin<<" to "<<xMax<<", range:"<<
	            yMin<<" to "<<yMax<<" with a step size in the x of:"<<xStepSize<<", and in the y: "<<yStepSize<<"\n";
	    for(int i =0; i<functions.size();i++)
	    {
	        std::cout<<"Function: "<<functions[i]<<", plotted using symbol: "<<symbols[i%10]<<", function defined as: "<<(*memory)[functions[i]].instructionsToFormattedString();
	        std::cout<<"Area Under Curve calculated with reimann sum using "<<tableWidth<<" partitions: "<<sumArea[i]<<"\n\n";
	    }
		return MAX;
	}
};

#endif /* KEYWORDS_PLOTACTION_HPP_ */
