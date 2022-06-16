/*
 * NewLocalVarAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_NEWLOCALVARACTION_HPP_
#define KEYWORDS_NEWLOCALVARACTION_HPP_

#include "../Keyword.hpp"
class NewLocalVarAction: public StKeyword {
public:
	NewLocalVarAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "cloc";
	}
	/*
	void action(AscalFrame<double>* frame) override
	{
		uint32_t index = frame->index+4;
	    SubStr localName = ParsingUtil::getVarNameSV(frame->exp, index);
	    index = frame->exp.find('=',frame->index)+1;
		SubStr subexp = ParsingUtil::getExpr(frame->exp, frame->exp.find('=',frame->index)+1, runtime.ascal_cin);
	    Object *nobj = nullptr;
	    double value = 0;
	    if(!ParsingUtil::isDouble(subexp.data))
	    {
		    value = (runtime.callOnFrame(frame, subexp.data));
		    Object newLocalVar(runtime.memMan, localName.data);
		    newLocalVar.setDouble(value);
		    nobj = &runtime.loadUserDefinedFn(newLocalVar, *frame->getLocalMemory());
	    }
	    else
	    {
	    	Object obj(runtime.memMan, localName.data);
    		char tmp = subexp.data[subexp.data.size()];
    		subexp.data[subexp.data.size()] = 0;
    		obj.setDouble(atof(&subexp.data[0]));
    		subexp.data[subexp.data.size()] = tmp;
		    nobj = &runtime.loadUserDefinedFn(obj, *frame->getLocalMemory());
	    }
	    if(*runtime.boolsettings["o"])
	    {
	        std::cout<<std::endl<<"New local var: "<<localName.data<< " = "<<value<<std::endl;
	    }
	}
*/};

#endif /* KEYWORDS_NEWLOCALVARACTION_HPP_ */
