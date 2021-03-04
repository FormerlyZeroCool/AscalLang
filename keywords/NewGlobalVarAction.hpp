/*
 * NewVerAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALVARACTION_HPP_
#define KEYWORDS_NEWGLOBALVARACTION_HPP_

#include "../Keyword.hpp"
class NewGlobalVarAction: public StKeyword {
public:
	NewGlobalVarAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "const";
	}
	void action(AscalFrame<double>* frame) override
	{
		SubStr exPart = ParsingUtil::getExpr(frame->exp, frame->exp.find('=',frame->index)+1, runtime.ascal_cin);
		    SubStr newVarPart = ParsingUtil::getVarName(frame->exp,frame->index+5);
	        Object *parent = nullptr;
	        if(frame->getContext())
	        	parent = runtime.resolveNextObjectExpressionPartial(frame, newVarPart, frame->getContext()->getThis());
		    Object var(runtime.memMan, newVarPart.data);

	        if(parent)
	        {
			    Object *newObj = nullptr;
	        	if(ParsingUtil::isDouble(var.id))
	        		newObj = &parent->setList(var, stoi(var.id));
	        	else
	        		newObj = &parent->loadChild(var, runtime);

	        	Object *c = frame->getContext();
	        	frame->setContext(newObj);
	    	    double value = runtime.callOnFrame(frame, exPart.data);
	        	frame->setContext(c);
	    	    newObj->setDouble(value);
	        }
	        else if(ParsingUtil::isDouble(exPart.data))
	        {
	    		char tmp = exPart.data[exPart.data.size()];
	    		exPart.data[exPart.data.size()] = 0;
	    		var.setDouble(atof(&exPart.data[0]));
	    		exPart.data[exPart.data.size()] = tmp;
			    runtime.loadUserDefinedFn(var, *frame->getLocalMemory());
	        }
	        else
	        {
	    	    double value = runtime.callOnFrame(frame, exPart.data);
	    	    var.setDouble(value);
	        	//delete old  reference from list not really needed
	    	    std::vector<Object>::iterator position = std::find(runtime.userDefinedFunctions.begin(), runtime.userDefinedFunctions.end(), runtime.memory[var.id]);
	    	                            if(position != runtime.userDefinedFunctions.end())
	    	                            	runtime.userDefinedFunctions.erase(position);
	    	    //set var defined's value in hash map
	    	                            runtime.loadUserDefinedFn(var, runtime.memory);
	    	                            runtime.userDefinedFunctions.push_back(var);
	        }
		    if(*runtime.boolsettings["o"])
		    {
		        std::cout<<std::endl<<"New global var: "<<newVarPart.data<< " = "
		        		<<var.instructionsToFormattedString()<<std::endl;
		    }
	}
};

#endif /* KEYWORDS_NEWVERACTION_HPP_ */
