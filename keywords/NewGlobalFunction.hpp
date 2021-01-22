/*
 * NewGlobalFunction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWGLOBALFUNCTION_HPP_
#define KEYWORDS_NEWGLOBALFUNCTION_HPP_

#include "../Keyword.hpp"
class NewGlobalFunction: public Keyword {
public:
	NewGlobalFunction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "let";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	                SubStr exPart = ParsingUtil::getExpr(frame->exp,frame->exp.find('=',frame->index)+1,runtime->ascal_cin);
	                SubStr newVarPart = ParsingUtil::getVarName(frame->exp, frame->index+4);
	                Object *parent = runtime->resolveNextObjectExpressionPartial(frame, newVarPart);
	                Object var(newVarPart.data,exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1 ));
	                if(parent)
	                {
	                	if(ParsingUtil::isDouble(var.id))
	                		parent->setList(var, stoi(var.id));
	                	else
	                		parent->loadChild(var);
	                }
	                else if(memory->count(var.id) != 0)
	                {
	                    std::vector<Object>::iterator position = std::find(runtime->userDefinedFunctions.begin(), runtime->userDefinedFunctions.end(), (*memory)[var.id]);
	                    if(position != runtime->userDefinedFunctions.end())
	                        runtime->userDefinedFunctions.erase(position);
	                }
	                //set var defined's value in hashmap
		    	    //set var defined's value in hash map
		    	                            (*memory)[var.id] = var;
		    	                            runtime->userDefinedFunctions.push_back(var);
	                if(*(*boolsettings)["o"])
	                {
	                    std::cout<<std::endl<<"New global function: "<<var.id
	                    		<< " exp: "<<var.instructionsToFormattedString()<<std::endl;
	                }
	                return MAX;
	}
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
