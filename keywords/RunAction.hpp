/*
 * RunAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_RUNACTION_HPP_
#define KEYWORDS_RUNACTION_HPP_

#include "../Keyword.hpp"
#include "FileHandler.hpp"
class RunAction: public StKeyword {
private:
public:
	RunAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "run";
	}
	void action(AscalFrame<double>* frame) override
	{
	    try{
		    if(*runtime.boolsettings["o"])
	        {
	        	std::cout<<"Attempting to run file\n";
	        }
	        FileHandler::loadFile(runtime, frame->exp, 4, runtime.ascal_cin);
		    if(*runtime.boolsettings["o"])
	        {
	        	std::cout<<"Finished running file\n";
	        }
	    }catch(std::string &exception)
	    {
	        throw std::string("while running file\n"+exception);
	    }
	}
};

#endif /* KEYWORDS_RUNACTION_HPP_ */
