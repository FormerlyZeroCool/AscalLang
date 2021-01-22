/*
 * UndoAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_UNDOACTION_HPP_
#define KEYWORDS_UNDOACTION_HPP_

#include "../Keyword.hpp"
class UndoAction: public Keyword {
public:
	UndoAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "u";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    double value = 0;
	    if(!runtime->lastExp.isEmpty()){
	        std::string last = "";
	        runtime->lastExp.top(last);
	        runtime->lastExp.pop();
	        runtime->undoneExp.push(last);
	        std::cout<<last<<std::endl;
	        value = runtime->callOnFrame(frame,"print "+last);
	    }
	    else
	        std::cout<<"No previous statements"<<std::endl;
	    return MAX;
	}
};

#endif /* KEYWORDS_UNDOACTION_HPP_ */
