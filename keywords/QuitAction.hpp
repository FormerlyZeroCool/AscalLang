/*
 * QuitAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_QUITACTION_HPP_
#define KEYWORDS_QUITACTION_HPP_

#include "../Keyword.hpp"
class QuitAction: public StKeyword {
public:
	QuitAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "quit";
	}
	void action(AscalFrame<double>* frame) override
	{
	    if(*runtime.boolsettings["o"])
	        std::cout<<"Quitting Ascal, have a nice day!"<<std::endl;
	    throw 0;
	}
};

#endif /* KEYWORDS_QUITACTION_HPP_ */
