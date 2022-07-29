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
#include <utility>
#include <array>

struct GuiPlotParams;
inline void runGuiPlot(KeywordExecutionContext ctx);
class PlotGUIAction: public StKeyword {
private:
    ParsedStatementList params;
public:
	static inline const uint64_t LOCAL_FUNCTION = 0, GLOBAL_FUNCTION = 1;
	static inline const uint64_t MAX_FUNCTIONS = 16, TABLE_WIDTH = 4096;
	PlotGUIAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "plotGUI";
	}
	void compile (CompilationContext &ctx) override
	{
	    SubStrSV exp = ParsingUtil::getFollowingExprSV(ctx.source, ctx.src_index, this->keyWord);
	    ParsingUtil::ParseStatementList(exp.data, 0, params);
	    if(params.statements.size() < 3)
	    	throw std::string("plotGUI <fun1|fun2...>,<x lower bound>, <x upper bound>");
	    std::string s = params.statements[0].data.str();
	    std::vector<std::string> functions = ParsingUtil::split(s, std::string("|"));
        for(uint32_t j = 1; j < params.statements.size(); j++)
        {
            ctx.target.compileParams(params.statements[j].data, ctx.runtime, ctx);
			//std::cout<<"compiled: "<<params.statements[j].data<<" code len: "<<ctx.target.getInstructions().size()<<"\n";
        }
		this->operation = runGuiPlot;
		ctx.target.append(this->operation);
		ctx.target.append((uint64_t) functions.size());
		if(functions.size() > MAX_FUNCTIONS)
		{
			throw std::string("Error too many functions, can only render: " + ParsingUtil::to_string(PlotGUIAction::MAX_FUNCTIONS) + (" functions at once."));
		}
		for(uint64_t i = 0; i < functions.size(); i++)
		{
			const auto localObjIt = ctx.localMemory.find(functions[i]);
			if(localObjIt != ctx.localMemory.end())
			{
				const CompilationContext::LocalRecord localRec = (*localObjIt).getValue();
				ctx.target.append(LOCAL_FUNCTION);
				ctx.target.append((uint64_t) localRec.stack_index);
			}
			else
			{
				const auto globalRec = ctx.runtime.memory.find(functions[i]);
				if(globalRec != ctx.runtime.memory.end())
				{
					const Object* function = (*globalRec).getValue();
					ctx.target.append(GLOBAL_FUNCTION);
					ctx.target.append(function);
				}
				else
				{
					//compile keyword call into local function
					const uint64_t stack_index = ctx.addOwnedLocal(string_view("x", 0), 1);
					ctx.target.append(LOCAL_FUNCTION);
					ctx.target.append(stack_index);
				}
			}
		}

	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"plotGUI"<<exp.data.substr(0,exp.data.size()-2)<<"\n";
	    }
	}	
};
struct GuiPlotParams {
	uint64_t functionCount;
	double xMin = 0;
	double xMax = 0;
	std::array<Object*, PlotGUIAction::MAX_FUNCTIONS> functions;
	Vect2D<float> results;
	GuiPlotParams(): results(0,0) {}
	GuiPlotParams(KeywordExecutionContext ctx): results(0,0) //loads data from vm into struct, and increments instruction pointer
	{
		this->loadDataFromVM(ctx);
	}
	void loadDataFromVM(KeywordExecutionContext ctx)
	{
		AscalExecutor::Operand xMin, xMax;
		ctx.frame().index += Keyword::opcodeSize();
		//loading xMin, and xMax from stack because they were dynamically calculated from expressions
		ctx.frame().initialOperands.top(xMax);
		ctx.frame().initialOperands.pop();
		ctx.frame().initialOperands.top(xMin);
		ctx.frame().initialOperands.pop();
		this->xMax = xMax.number();
		this->xMin = xMin.number();
		ctx.getData(functionCount, ctx.frame().index);
		ctx.frame().index += sizeof(uint64_t);
		uint64_t i = 0;
		for(; i < functionCount; i++)//load object pointers into memory
			{
				uint64_t type = 0;
				ctx.getData(type, ctx.frame().index);
				ctx.frame().index += sizeof(type);
				if(type == PlotGUIAction::LOCAL_FUNCTION)
				{
					uint64_t stackIndex = 0;
					ctx.getData(stackIndex, ctx.frame().index);
					ctx.frame().index += sizeof(stackIndex);
					functions[i] = ctx.frame().localMemory[stackIndex].data.obj;
				}
				else//load global
				{
					ctx.getData(functions[i], ctx.frame().index);
					ctx.frame().index += sizeof(functions[i]);
				}
			}
		results.reserve(PlotGUIAction::TABLE_WIDTH * functionCount);
		results.set(PlotGUIAction::TABLE_WIDTH, functionCount);
		recalcResults(ctx);
	}
	double getdx() const noexcept
	{
		return (xMax - xMin * 1.0) / PlotGUIAction::TABLE_WIDTH;
	}
	Vect2D<float>& recalcResults(KeywordExecutionContext ctx) noexcept
	{
		const double dx = getdx();
		for(int j = 0; j < results.getHeight();j++)
		{
		    const Object* function = functions[j];
			double xi = xMin;
		    for(int i = 0; i < results.getWidth();i++)
		    {
				AscalFrame<double> frame(ctx.runtime());
				frame.exp = function->getInstructions().substr(0, function->getInstructions().size() - sizeof(void*));
				frame.localMemory.push(StackDataRecord(StackDataRecord::DOUBLE, xi));
				const double result = ctx.runtime().calculateExpression(&frame);
				//std::cout<<" "<<result<<", ";
				results.insertAt(i, j, result);
				frame.initialOperands.pop();
				frame.localMemory.pop();
				xi += dx;
			}
			//std::cout<<"\n";
		}
		return results;
	}
};
inline void runGuiPlot(KeywordExecutionContext ctx)
{
	GuiPlotParams parameters(ctx);
	//your gui code here, to recalculate for different range just change xMin, and xMax, and call recalcResults
	//for the vec2d each y will be one functions values from the left to the right of the array the x values are 
	//ascending by parameters.getdx()
	//the leftmost x value will be xMin, and each successor will be xMin + parameters.getdx() * (i + 1)
}
#endif /* KEYWORDS_PLOTGUIACTION_HPP_ */
