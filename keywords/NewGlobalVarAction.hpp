/*
 * NewVerAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALVARACTION_HPP_
#define KEYWORDS_NEWGLOBALVARACTION_HPP_

#include "../Keyword.hpp"
class NewGlobalVarAction: public Keyword {
public:
	NewGlobalVarAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "const";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		SubStr exPart = ParsingUtil::getExpr(frame->exp, frame->exp.find('=',frame->index)+1, runtime->ascal_cin);
		    SubStr newVarPart = ParsingUtil::getVarName(frame->exp,frame->index+5);
	        Object *parent = runtime->resolveNextObjectExpressionPartial(frame, newVarPart, frame->getContext()->getThis());
		    Object var(newVarPart.data,"","");
		    Object *newObj = nullptr;

	        if(parent)
	        {
	        	if(ParsingUtil::isDouble(var.id))
	        		newObj = &parent->setList(var, stoi(var.id));
	        	else
	        		newObj = &parent->loadChild(var);

	        	Object *c = frame->getContext();
	        	frame->setContext(newObj);
	    	    std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame, exPart.data));
	        	frame->setContext(c);
	    	    newObj->getInstructions() = value;
	        }
	        else
	        {
	    	    std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame, exPart.data));
	    	    var.getInstructions() = value;
	        	//delete old  reference from list not really needed
	    	    std::vector<Object>::iterator position = std::find(runtime->userDefinedFunctions.begin(), runtime->userDefinedFunctions.end(), (*memory)[var.id]);
	    	                            if(position != runtime->userDefinedFunctions.end())
	    	                            	runtime->userDefinedFunctions.erase(position);
	    	    //set var defined's value in hash map
	    	                            (*memory)[var.id] = var;
	    	                            runtime->userDefinedFunctions.push_back(var);
	        }
		    if(*(*boolsettings)["o"])
		    {
		        std::cout<<std::endl<<"New global var: "<<newVarPart.data<< " = "
		        		<<var.instructionsToFormattedString()<<std::endl;
		    }
		    return MAX;
	}
};

#endif /* KEYWORDS_NEWVERACTION_HPP_ */
