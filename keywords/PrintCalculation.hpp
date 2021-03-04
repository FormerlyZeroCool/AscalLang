/*
 * PrintCalculation.hpp
 *
 *  Created on: Jan 17, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTCALCULATION_HPP_
#define KEYWORDS_PRINTCALCULATION_HPP_

#include "../Keyword.hpp"
class PrintCalculation: public StKeyword {
public:
	PrintCalculation(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "printc";
	}
	void action(AscalFrame<double>* frame) override
	{
	    //the 6 added here could be 5 as print is only 5 char long,
	    //but I also want to accomodate printc sending unaltered statements to this function,
	    //and it's not like the expression itself can start at 5+ the index of print,
	    //that must begin at 6+ the index of the p in print at least
	    std::string exp = ParsingUtil::getExpr(frame->exp.substr(frame->index+6,frame->exp.length()), 0, runtime.ascal_cin).data;
	    bool print = *runtime.boolsettings["p"];
	    *runtime.boolsettings["p"] = true;
	    runtime.callOnFrameFormatted(frame, exp);
	    *runtime.boolsettings["p"] = print;
	}
};

#endif /* KEYWORDS_PRINTCALCULATION_HPP_ */
