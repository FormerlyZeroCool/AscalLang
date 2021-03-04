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
class ImportAction: public StKeyword {
private:
public:
	ImportAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "import";
	}
	void action(AscalFrame<double>* frame) override
	{
	    try{
		    if(*runtime.boolsettings["o"])
	        {
	        	std::cout<<"Attempting to run file\n";
	        }
	    	FileHandler::loadFile(runtime, frame->exp,keyWord.length(), runtime.ascal_cin);
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

#endif /* KEYWORDS_IMPORTACTION_HPP_ */
