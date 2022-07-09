/*
 * QuitAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_QUITACTION_HPP_
#define KEYWORDS_QUITACTION_HPP_

#include "../Keyword.hpp"
	static inline void quitAction(KeywordExecutionContext ctx) 
	{
	    if(*ctx.runtime().boolsettings["o"])
	        std::cout<<"Quitting Ascal, have a nice day!"<<std::endl;
	    throw 0;
	}
class QuitAction: public StKeyword {
public:
	QuitAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "quit";
		this->operation = quitAction;
	}
};

#endif /* KEYWORDS_QUITACTION_HPP_ */
