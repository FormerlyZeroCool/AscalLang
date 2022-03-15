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
        uint32_t exStart = frame->exp.find("=",frame->index)+1;
        SubStr exPart = ParsingUtil::getExpr(frame->exp, exStart, runtime.ascal_cin);
        uint32_t index = frame->index+4;
        SubStrSV newVarPart = ParsingUtil::getVarNameSV(frame->exp, index);
        Object *parent = runtime.resolveNextObjectExpressionPartial(frame, newVarPart);
        Object *obj = nullptr;
        if(parent)
        {
            if(ParsingUtil::getFirstChar(newVarPart.data) == '\"'){
                Object var(runtime.memMan, newVarPart.data,exPart.data);
                //,frame->exp.substr(newVarPart.end + 1,exPart.start - 1).str()
                var.compileInstructions();
                obj = &parent->setList(var, atoi(var.getId().c_str()));
            }
            else if(ParsingUtil::getFirstChar(exPart.data) == '[')
            {
                Object var(runtime.memMan, newVarPart.data,exPart.data);
                Object pushMethod(runtime.memMan, string_view("push"), string_view("arrPush(this,numberzxa)"));
                obj = &parent->loadChild(var, runtime);
                obj->loadChild(pushMethod, runtime);
            }
            else{
                index = frame->index;
                Object var(runtime.memMan, newVarPart.data,exPart.data);
                var.compileInstructions();
                obj = &parent->loadChild(var, runtime);
            }
        }
        else
        {
            if(runtime.memory.count(newVarPart.data) != 0)
            {
                std::vector<Object>::iterator position = std::find(runtime.userDefinedFunctions.begin(), runtime.userDefinedFunctions.end(), (runtime.memory)[newVarPart.data]);
                if(position != runtime.userDefinedFunctions.end())
                    runtime.userDefinedFunctions.erase(position);
            }
            Object var(runtime.memMan, newVarPart.data,exPart.data);
            string_view s = frame->exp.substr(exStart,frame->exp.length() - exStart);
            var.compileInstructions();
            obj = &runtime.loadUserDefinedFn(var, runtime.memory);
            if(ParsingUtil::getFirstChar(s) == '[')
            {
                Object pushMethod(runtime.memMan, string_view("push"), string_view("arrPush(this,numberzxa)"));
                pushMethod.compileInstructions();
                Object popMethod(runtime.memMan, string_view("pop"), string_view("arrErase(this,arrLen(this)-1)"));
                popMethod.compileInstructions();
                Object eraseMethod(runtime.memMan, string_view("erase"), string_view("arrErase(this,index)"));
                popMethod.compileInstructions();
                obj->loadChild(pushMethod, runtime);
                obj->loadChild(popMethod, runtime);
                obj->loadChild(eraseMethod, runtime);
            }
        }
        if(*runtime.boolsettings["o"])
        {
            std::cout<<std::endl<<"New global function: "<<obj->getId()
                    << " exp: "<<obj->instructionsToFormattedString()<<std::endl;
        }
	}
};

#endif /* KEYWORDS_NEWGLOBALFUNCTION_HPP_ */
