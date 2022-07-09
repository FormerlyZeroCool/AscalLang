/*
 * NewLocalVar.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWLOCALVAR_HPP_
#define KEYWORDS_NEWLOCALVAR_HPP_

#include "../Keyword.hpp"
class NewLocalVar: public StKeyword {
public:
    NewLocalVar(AscalExecutor &runtime):
    StKeyword(runtime)
    {
        this->keyWord = "loc";
    }
    void action(AscalFrame<double>* frame) override
    {
        Object *nobj = nullptr;
        SubStr localName = ParsingUtil::getVarName(frame->exp,frame->exp.find("loc",frame->index)+4);
        static uint32_t startOfExp;
        startOfExp = frame->exp.find('=', frame->index)+1;
        const char firstCharExp = ParsingUtil::getFirstChar(frame->exp.substr(startOfExp));
        if(firstCharExp == '[')
        {
            Object obj(runtime.memMan, localName.data);
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
            runtime.makeArray(*nobj);
        }
        else if(firstCharExp == '\"')
        {
            Object obj(runtime.memMan, localName.data);
            nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
            runtime.makeString(*nobj);
        }
        else
        {
            SubStr subexp = ParsingUtil::getExpr(frame->exp, startOfExp, runtime.ascal_cin);
            if(!ParsingUtil::isObj(subexp.data))
            {
                if(!ParsingUtil::isDouble(subexp.data))
                {
                    Object newLocal = Object(runtime.memMan, localName.data,subexp.data,"");
                    newLocal.compileInstructions();
                    nobj = &runtime.loadUserDefinedFn(newLocal, *frame->getLocalMemory());
                }
                else
                {
                    Object obj(runtime.memMan, localName.data);
                    const uint32_t nullIndex = ParsingUtil::isNumeric(subexp.data[subexp.data.size()-1])?subexp.data.size():subexp.data.size()-1;
                    char tmp = subexp.data[nullIndex];
                    subexp.data[nullIndex] = 0;
                    obj.setDouble(atof(&subexp.data[0]));
                    subexp.data[nullIndex] = tmp;
                    nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
                }
                if(*runtime.boolsettings["o"])
                {
                    std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<nobj->instructionsToFormattedString()<<std::endl;
                }
            }
            else
            {
                SubStrSV rightVarStart = ParsingUtil::getVarNameSV(frame->exp,startOfExp);
                Object *newVar = runtime.resolveNextObjectExpression(frame, rightVarStart).data;
                if(newVar)
                {
                    runtime.loadUserDefinedFn(*newVar, *frame->getLocalMemory());
                }
                else
                {
                    Object newLocal = Object(runtime.memMan, localName.data,subexp.data, "");
                    newLocal.compileInstructions();
                    runtime.loadUserDefinedFn(newLocal, *frame->getLocalMemory());
                }
                if(*runtime.boolsettings["o"])
                {
                    std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<newVar->toString()<<std::endl;
                }
            }

        }

    }
};

#endif /* KEYWORDS_NEWLOCALVAR_HPP_ */
