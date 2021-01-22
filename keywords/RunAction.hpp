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
class RunAction: public Keyword {
private:
public:
	RunAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "run";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    try{
	        if(*(*boolsettings)["o"])
	        {
	        	std::cout<<"Attempting to run file\n";
	        }
	        FileHandler::loadFile(runtime, frame->exp, 4, runtime->ascal_cin);
	        if(*(*boolsettings)["o"])
	        {
	        	std::cout<<"Finished running file\n";
	        }
	    }catch(std::string &exception)
	    {
	        throw std::string("while running file\n"+exception);
	    }
	    return MAX;
	}
};

#endif /* KEYWORDS_RUNACTION_HPP_ */
