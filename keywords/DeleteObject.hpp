/*
 * DeleteObject.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_DELETEOBJECT_HPP_
#define KEYWORDS_DELETEOBJECT_HPP_

#include "../Keyword.hpp"
class DeleteObject: public Keyword {
public:
	DeleteObject(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "delete";
	}
	std::string deleteObject(AscalFrame<double>* frame,bool saveLast)
	{

	    if(ParsingUtil::cmpstr(frame->exp.substr(7,3),std::string("all")))
	    {
	        memory->clear();
	        frame->getLocalMemory()->clear();
	        runtime->userDefinedFunctions.clear();
	        if(*(*boolsettings)["o"])
	        	std::cout<<"All Memory cleared."<<std::endl;
	    }
	    else
	    {
	        std::string name = ParsingUtil::getVarName(frame->exp,7).data;

	        if(memory->count(name) > 0)
	        {

	            std::vector<Object>::iterator position = std::find(runtime->userDefinedFunctions.begin(), runtime->userDefinedFunctions.end(), (*memory)[name]);
	            if(position != runtime->userDefinedFunctions.end())
	                runtime->userDefinedFunctions.erase(position);

	            memory->erase(name);
	            if(*(*boolsettings)["o"])
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
	    return MAX;
	}
};

#endif /* KEYWORDS_DELETEOBJECT_HPP_ */
