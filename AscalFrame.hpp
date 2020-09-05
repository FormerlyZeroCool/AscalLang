/*
 * AscalFrame.h
 *
 *  Created on: Jun 29, 2020
 *      Author: andrewrubinstein
 */

#ifndef ASCALFRAME_HPP_
#define ASCALFRAME_HPP_
#include <string>
#include <map>

#include "Object.hpp"
#include "stack.hpp"

template <typename t>
class AscalFrame {
private:

protected:
	//+1 sets if result flag to true, +8 sets isDynamicAllocation true
	uint8_t flagRegisters = 1 + 8;
    AscalParameters* params;
    std::map<std::string,Object>* paramMemory;
    std::map<std::string,Object>* localMemory;

public:
	AscalFrame* returnPointer = nullptr;
    uint8_t level = 0;
	uint32_t stackIndex = 0;
	uint32_t index = 0;
	uint64_t memoPointer = 0;
	std::string exp;
	stack<t> initialOperands;
	stack<char> initialOperators;
    AscalFrame<t>*  getReturnPointer()
    {
    	return returnPointer;
    }
	//The retrieve data from flag registers functions bitmask all the bits that don't represent the data the user wishes to retreive
    //Then auto cast the byte to a bool
    bool ifResultFlag() { return flagRegisters & (uint8_t)1; };
    bool ifFlag() { return flagRegisters & (uint8_t)2; };
    bool comingfromElse() { return flagRegisters & (uint8_t)4; };
    bool isDynamicAllocation() { return flagRegisters & (uint8_t)8; };
    bool isRoot() { return flagRegisters & (uint8_t)16; };
    bool isFunction() { return flagRegisters & (uint8_t)32; };
    void setIfResultFlag(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)254;
        flagRegisters = flagRegisters^(uint8_t)(1*value);
    };
    void setIfFlag(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)253;
        flagRegisters = flagRegisters^(uint8_t)(2*value);
    };
    void setComingfromElse(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)251;
        flagRegisters = flagRegisters^(uint8_t)(4*value);
    };
    void setIsDynamicAllocation(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)(255-8);
        flagRegisters = flagRegisters^(uint8_t)(8*value);
    };
    void setIsRoot(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)(255-16);
        flagRegisters = flagRegisters^(uint8_t)(16*value);
    };
    std::map<std::string,Object>* getParamMemory(){ return paramMemory; };
    std::map<std::string,Object>* getLocalMemory(){ return localMemory; };
    AscalParameters* getParams(){ return params; };
	virtual
	void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) {};
	virtual
	~AscalFrame() {};
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
    	//+1 sets if result flag to true, +8 sets isDynamicAllocation true
        //+32 sets isFunction true
        this->flagRegisters = 1 + 8 + 32;
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

#endif /* ASCALFRAME_HPP_ */
