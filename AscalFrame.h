/*
 * AscalFrame.h
 *
 *  Created on: Jun 29, 2020
 *      Author: andrewrubinstein
 */

#ifndef ASCALFRAME_H_
#define ASCALFRAME_H_
#include <string>
#include <map>
#include "stack.h"
#include "Object.h"

template <typename t>
class AscalFrame {
public:
	bool isDynamicAllocation = true;
	bool printValue;
	uint32_t index = 0;
	uint32_t stackIndex = 0;
	AscalFrame* returnPointer = nullptr;
	std::string exp;
	stack<t> initialOperands;
	stack<char> initialOperators;
    std::map<std::string,Object>* getParamMemory(){ return paramMemory; };
    std::map<std::string,Object>* getLocalMemory(){ return localMemory; };
    AscalParameters* getParams(){ return params; };
	virtual
	void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) {};
	virtual
	~AscalFrame() {};
    protected:
        AscalParameters* params;
        std::map<std::string,Object>* paramMemory;
        std::map<std::string,Object>* localMemory;
};
template <typename t>
class ParamFrame: public AscalFrame<t> {
public:
    ParamFrame(AscalParameters* params, std::map<std::string,Object>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
    }
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
    {
        if(this->returnPointer)
        {
            this->returnPointer->getParams()->push_back(std::to_string(result));
            /*std::cout<<"From Ascal PFrame "<<result<<" to params\n";
            std::cout<<"    returned from index: "<<this->stackIndex<<" exp: "<<this->exp<<"\n";
            std::cout<<"    returned to index: "<<this->returnPointer->stackIndex<<" exp: "<<this->returnPointer->exp<<"\n";
        */}
    }
    ~ParamFrame(){
        //std::cout<<"from ascalframe.h Param popped\n";
    };
};
template <typename t>
class ParamFrameFunctionPointer: public AscalFrame<t> {
public:
    std::string functionName;
    ParamFrameFunctionPointer(AscalParameters* params, std::map<std::string,Object>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
    }
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
    {
        if(this->returnPointer)
        {
            std::string fnBody;
            if(this->returnPointer->getLocalMemory()->count(functionName))
            {
                fnBody = (*this->returnPointer->getLocalMemory())[functionName].getInstructions();
            }
            else if(this->returnPointer->getParamMemory()->count(functionName))
            {
                fnBody = (*this->returnPointer->getParamMemory())[functionName].getInstructions();
            }
            else if(globalMemory.count(functionName))
            {
                fnBody = globalMemory[functionName].getInstructions();
            }
            else
            {
                throw std::string("Error, could not find function "+functionName+" referenced.");
            }
            this->returnPointer->getParams()->push_back(fnBody);
            /*std::cout<<"From Ascal PFrame "<<fnBody<<" to params\n";
            std::cout<<"    returned from index: "<<this->stackIndex<<" exp: "<<this->exp<<"\n";
            std::cout<<"    returned to index: "<<this->returnPointer->stackIndex<<" exp: "<<this->returnPointer->exp<<"\n";
       */ }
    }
    ~ParamFrameFunctionPointer(){
        //std::cout<<"from ascalframe.h Param popped\n";
    };
};
template <typename t>
class FunctionFrame: public AscalFrame<t> {
public:
    FunctionFrame(AscalParameters* params, std::map<std::string,Object>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = new AscalParameters;
        this->paramMemory = new std::map<std::string,Object>;
        this->localMemory = new std::map<std::string,Object>;
    }
	void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
	{
		if(this->returnPointer)
		{
			this->returnPointer->initialOperands.push_back(result);
            /*std::cout<<"From Ascal Frame "<<result<<" to stack\n";
			std::cout<<"    returned from index: "<<this->stackIndex<<" exp: "<<this->exp<<"\n";
			std::cout<<"    returned to index: "<<this->returnPointer->stackIndex<<" exp: "<<this->returnPointer->exp<<"\n";
		*/}
	}
	~FunctionFrame(){
		//std::cout<<"from ascalframe.h Function popped\n";
        delete this->params;
        delete this->paramMemory;
        delete this->localMemory;
	}
};

#endif /* ASCALFRAME_H_ */
