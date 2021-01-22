/*
 * QuitAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_QUITACTION_HPP_
#define KEYWORDS_QUITACTION_HPP_

#include "../Keyword.hpp"
class QuitAction: public Keyword {
public:
	QuitAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "quit";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    if(*(*boolsettings)["o"])
	        std::cout<<"Quitting Ascal, have a nice day!"<<std::endl;
	    throw 0;
	    return MAX;
	}
};

#endif /* KEYWORDS_QUITACTION_HPP_ */
