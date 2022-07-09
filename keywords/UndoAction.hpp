/*
 * UndoAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_UNDOACTION_HPP_
#define KEYWORDS_UNDOACTION_HPP_

#include "../Keyword.hpp"
class UndoAction: public StKeyword {
public:
	UndoAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "u";
	}
	void action(AscalFrame<double>* frame) override
	{
	    double value = 0;
	    if(!runtime.lastExp.isEmpty()){
	        std::string last = "";
	        runtime.lastExp.top(last);
	        runtime.lastExp.pop();
	        runtime.undoneExp.push(last);
	        std::cout<<last<<std::endl;
	        value = runtime.callOnFrame(frame,"print "+last);
	    }
	    else
	        std::cout<<"No previous statements"<<std::endl;
	}
};

#endif /* KEYWORDS_UNDOACTION_HPP_ */
