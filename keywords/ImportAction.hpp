/*
 * ImportAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_IMPORTACTION_HPP_
#define KEYWORDS_IMPORTACTION_HPP_


#include "../Keyword.hpp"
#include "FileHandler.hpp"
class ImportAction: public Keyword {
private:
public:
	ImportAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "import";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    try{
	    	if(*(*boolsettings)["o"])
	        {
	        	std::cout<<"Attempting to run file\n";
	        }
	    	FileHandler::loadFile(runtime, frame->exp,keyWord.length(), runtime->ascal_cin);
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

#endif /* KEYWORDS_IMPORTACTION_HPP_ */
