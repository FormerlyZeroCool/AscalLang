/*
 * NewGlobalFunction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALFUNCTION_HPP_
#define KEYWORDS_NEWGLOBALFUNCTION_HPP_

#include "../Keyword.hpp"
class NewGlobalFunction: public StKeyword {
public:
	NewGlobalFunction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "let";
	}
	void action(AscalFrame<double>* frame) override
	{
	                SubStr exPart = ParsingUtil::getExpr(frame->exp,frame->exp.find('=',frame->index)+1,runtime.ascal_cin);
	                SubStr newVarPart = ParsingUtil::getVarName(frame->exp, frame->index+4);
	                Object *parent = runtime.resolveNextObjectExpressionPartial(frame, newVarPart);
	                Object var(runtime.memMan, newVarPart.data,exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str());
	                var.compileInstructions();
	                Object *obj = nullptr;
	                if(parent)
	                {
	                	if(ParsingUtil::isDouble(var.id))
	                		obj = &parent->setList(var, stoi(var.id));
	                	else
	                		obj = &parent->loadChild(var, runtime);
	                }
	                else if(runtime.memory.count(var.id) != 0)
	                {
	                    std::vector<Object>::iterator position = std::find(runtime.userDefinedFunctions.begin(), runtime.userDefinedFunctions.end(), (runtime.memory)[var.id]);
	                    if(position != runtime.userDefinedFunctions.end())
	                        runtime.userDefinedFunctions.erase(position);
			    	    obj = &runtime.loadUserDefinedFn(var, runtime.memory);
	                }
	                else
	                {
			    	    obj = &runtime.loadUserDefinedFn(var, runtime.memory);
	                }
		    	    if(*runtime.boolsettings["o"])
	                {
	                    std::cout<<std::endl<<"New global function: "<<obj->id
	                    		<< " exp: "<<obj->instructionsToFormattedString()<<std::endl;
	                }
	}
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
