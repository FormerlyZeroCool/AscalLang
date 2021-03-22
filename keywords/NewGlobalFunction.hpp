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
        uint32_t index = frame->index+4;
        SubStrSV newVarPart = ParsingUtil::getVarNameSV(frame->exp, index);
        Object *parent = runtime.resolveNextObjectExpressionPartial(frame, newVarPart);
        Object *obj = nullptr;
        if(parent)
        {
            if(ParsingUtil::isDouble(newVarPart.data)){
                Object var(runtime.memMan, newVarPart.data.str(),exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str());
                var.compileInstructions();
                obj = &parent->setList(var, atoi(var.getId().c_str()));
            }
            else{
                index = frame->index;
                std::string newVarId = ParsingUtil::getVarName(frame->exp, index).data;
                Object var(runtime.memMan, newVarId,exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str());
                var.compileInstructions();
                obj = &parent->loadChild(var, runtime);
            }
        }
        else if(runtime.memory.count(newVarPart.data) != 0)
        {
            std::vector<Object>::iterator position = std::find(runtime.userDefinedFunctions.begin(), runtime.userDefinedFunctions.end(), (runtime.memory)[newVarPart.data]);
            if(position != runtime.userDefinedFunctions.end())
                runtime.userDefinedFunctions.erase(position);
            Object var(runtime.memMan, newVarPart.data.str(),exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str());
            var.compileInstructions();
            obj = &runtime.loadUserDefinedFn(var, runtime.memory);
        }
        else
        {
            Object var(runtime.memMan, newVarPart.data.str(),exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str());
            var.compileInstructions();
            obj = &runtime.loadUserDefinedFn(var, runtime.memory);
        }
        if(*runtime.boolsettings["o"])
        {
            std::cout<<std::endl<<"New global function: "<<obj->getId()
                    << " exp: "<<obj->instructionsToFormattedString()<<std::endl;
        }
	}
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
