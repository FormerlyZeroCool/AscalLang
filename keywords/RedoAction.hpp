/*
 * RedoAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_REDOACTION_HPP_
#define KEYWORDS_REDOACTION_HPP_

#include "../Keyword.hpp"
class RedoAction: public StKeyword {
public:
	RedoAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "r";
	}
	void action(AscalFrame<double>* frame) override
	{
	    std::string value = "0";
	    if(!runtime.undoneExp.isEmpty()){
	        std::string last = "";
	        runtime.undoneExp.top(last);
	        runtime.undoneExp.pop();
	        runtime.lastExp.push(last);
	        std::cout<<last<<std::endl;
	        value = runtime.callOnFrame(frame,"print "+last);
	    }
	    else
	        std::cout<<"No statements can be redone"<<std::endl;
	}
};

#endif /* KEYWORDS_REDOACTION_HPP_ */
