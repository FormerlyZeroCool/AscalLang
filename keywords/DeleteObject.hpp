/*
 * DeleteObject.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_DELETEOBJECT_HPP_
#define KEYWORDS_DELETEOBJECT_HPP_

#include "../Keyword.hpp"
class DeleteObject: public OpKeyword {
public:
	DeleteObject(AscalExecutor &runtime):
		OpKeyword(runtime)
	{
		this->keyWord = "delete";
	}
	void action(AscalFrame<double>* frame) override
	{

	    if(ParsingUtil::cmpstr(frame->exp.substr(7,3),std::string("all")))
	    {
	        runtime.memory.clear();
	        frame->getLocalMemory()->clear();
	        runtime.userDefinedFunctions.clear();
		    if(*runtime.boolsettings["o"])
	        	std::cout<<"All Memory cleared."<<std::endl;
	    }
	    else
	    {
	        std::string name = ParsingUtil::getVarName(frame->exp,7).data;

	        if(runtime.memory.count(name) > 0)
	        {

	            std::vector<Object>::iterator position = std::find(runtime.userDefinedFunctions.begin(), runtime.userDefinedFunctions.end(), (runtime.memory)[name]);
	            if(position != runtime.userDefinedFunctions.end())
	                runtime.userDefinedFunctions.erase(position);

	            runtime.memory.erase(name);
	    	    if(*runtime.boolsettings["o"])
	            	std::cout<<"Erased "<<name<<" from memory."<<std::endl;
	        }
	        else if(frame->getLocalMemory()->count(name) != 0)
	        {
	            frame->getLocalMemory()->erase(name);
	        }
	        else
	        {
	            throw std::string("Error could not find "+name+" in memory to delete.\n");
	        }
	    }
	}
};

#endif /* KEYWORDS_DELETEOBJECT_HPP_ */
