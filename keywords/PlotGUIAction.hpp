/*
 * PlotGUIAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PLOTGUIACTION_HPP_
#define KEYWORDS_PLOTGUIACTION_HPP_

#include "../Keyword.hpp"
#include "../Vect2D.hpp"
class PlotGUIAction: public Keyword {
public:
	PlotGUIAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "plotGUI";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		static const std::string keyWord = "plotGUI";
	    SubStr exp = ParsingUtil::getFollowingExpr(frame, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    if(params.size() < 7)
	    	throw std::string("plotGUI <fun1|fun2...>,<x lower bound>, <x upper bound>, <y upper bound>, <y lower bound>, <delta x, delta y>");
	    std::vector<std::string> functions = ParsingUtil::split(params[0].data, std::string("|"));
	    //Here to aid potential future development
	    double xMin = runtime->callOnFrame(frame, params[1].data);
	    double xMax = runtime->callOnFrame(frame, params[1].data);
	    double yMin = runtime->callOnFrame(frame, params[1].data);
	    double yMax = runtime->callOnFrame(frame, params[1].data);
	    double dx = runtime->callOnFrame(frame, params[1].data);
	    double dy = runtime->callOnFrame(frame, params[1].data);

	    //int pid = fork();
	    //if(!pid)
	    {
	    	//Essentially a 1d vector but you can address it with x, and y coordinates, y addresses different functions
	    	//x addresses the x point xMin+dx*x, using xMin, and dx defined above
	    	Vect2D<double> points = calcTable(functions, xMin, xMax, dx, dy);
	    	//Your GUI code
	    	//throw 0;
	    }
	    if(*(*boolsettings)["o"])
	    {
	    	std::cout<<"plotGUI"+exp.data.substr(0,exp.data.size()-2)+"\n";
	    }
	    //Automatically moves to next expression after keyword execution because we return MAX
	    return MAX;
	}
	//For plotGUI to use the y index of the Vect2D corresponds to the index of the function name in the functions vector
	//the x index in cartesian space is x*dx+xMin
	Vect2D<double> calcTable(const std::vector<std::string> &functions, double xMin, double xMax, double xStepSize, double yStepSize)
	{
	    int tableWidth = (xMax-xMin)/(xStepSize>0?xStepSize:1);
		double dx = (xMax-xMin)/tableWidth;
		    double dy = yStepSize>0?yStepSize:1;
		    double xi;
		    Vect2D<double> outPuts(tableWidth,functions.size()-1);
		    std::stringstream exp;
		    for(int j = 0;j<functions.size();j++)
		    {
		        std::string function = functions[j];
		        for(int i = 0;i<tableWidth;i++)
		        {
		            xi = xMin+dx*(i);
		            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
		            exp << function << '(' << ParsingUtil::to_string(xi) << ')';
		            calledFunction->exp = exp.str();
		            exp.str(std::string());
		            outPuts.push_back(
		                    runtime->calculateExpression(calledFunction));
		        }
		    }
		    return outPuts;
	}
};

#endif /* KEYWORDS_PLOTGUIACTION_HPP_ */
