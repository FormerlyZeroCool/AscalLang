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

#include "AscalParameters.hpp"
#include "Object.hpp"
#include "stack.hpp"
#include "Calculator.hpp"

template <typename t>
class AscalFrame {
private:

protected:
    //+1 sets if result flag to true, sp no else cases wil be initally executed
	//+8 sets isDynamicAllocation true, so that the calculateExpression functions is responsible for managin the memory allocated for this frame
	//+64 designates the frame as never run
    uint16_t flagRegisters = 1 + 8 + 64;
    AscalParameters* params;
    std::map<std::string,Object* >* paramMemory;
    std::map<std::string,Object>* localMemory;
    Object *contextObj = nullptr;

public:
    void setObjectReturnRegister(Object *obj)
    {
    	//set return flags so auto return does not occur, and upon returning to calling function it is aware an object has been returned
    	this->flagRegisters |= 256|512;
    	(*this->localMemory)["_rtn"] = *obj;
    }
    Object* getReturnObject()
    {
    	this->flagRegisters &= 65535-256-512;
    	return &(*this->localMemory)["_rtn"];
    }
    Object* getContext()
    {
    	return contextObj;
    }
    void setContext(Object *obj)
    {
    	this->contextObj = obj;
    }
    std::string memToString()
    {
    	std::stringstream s;
    	s<<"Params stack:\n";
    	for(auto &value:*params)
    	{
    		s<<value->toString()<<"\n";
    	}
    	s<<"Param memory:\n";
    	for(auto &[key, value]:*paramMemory)
    	{
    		s<<value->toString()<<"\n";
    	}
    	s<<"Local Memory\n";
    	for(auto &[key, value]:*localMemory)
    	{
    		s<<value.toString()<<"\n";
    	}
    	return s.str();
    }
    AscalFrame* returnPointer = nullptr;
    uint16_t level = 0;
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
    bool ifResultFlag() { return flagRegisters & (uint16_t)1; };
    bool ifFlag() { return flagRegisters & (uint16_t)2; };
    bool comingfromElse() { return flagRegisters & (uint16_t)4; };
    bool isDynamicAllocation() { return flagRegisters & (uint16_t)8; };
    bool isRoot() { return flagRegisters & (uint16_t)16; };
    bool isFunction() { return flagRegisters & (uint16_t)32; };
    bool isFirstRun() { return flagRegisters & (uint16_t)64; };
    bool isAugmented() { return flagRegisters & (uint16_t)128; };
    bool isReturnObjectFlagSet() { return flagRegisters & (uint16_t)256; };
    bool isReturnFlagSet() { return flagRegisters & (uint16_t)512; };
    void setIfResultFlag(bool value)
    {
        flagRegisters &= (uint16_t)65535-1;
        flagRegisters ^= (uint16_t)(1*value);
    };
    void setIfFlag(bool value)
    {
        flagRegisters &= (uint16_t)65535-2;
        flagRegisters ^= (uint16_t)(2*value);
    };
    void setComingfromElse(bool value)
    {
        flagRegisters &= (uint16_t)65535-4;
        flagRegisters ^= (uint16_t)(4*value);
    };
    void setIsDynamicAllocation(bool value)
    {
        flagRegisters &= (uint16_t)(65535-8);
        flagRegisters ^= (uint16_t)(8*value);
    };
    void setIsRoot(bool value)
    {
        flagRegisters &= (uint16_t)(65535-16);
        flagRegisters ^= (uint16_t)(16*value);
    };
    void setIsFirstRun(bool value)
    {
        flagRegisters &= (uint16_t)(65535-64);
        flagRegisters ^= (uint16_t)(64*value);
    };
    void setAugmented(bool value)
    {
        flagRegisters &= (uint16_t)(65535-128);
        flagRegisters ^= (uint16_t)(128*value);
    };
    void setReturnFlag(bool value)
    {
        flagRegisters &= (uint16_t)(65535-512);
        flagRegisters ^= (uint16_t)(512*value);
    };
    std::map<std::string,Object*>* getParamMemory(){ return paramMemory; };
    std::map<std::string,Object>* getLocalMemory(){ return localMemory; };
    AscalParameters* getParams(){ return params; };
    virtual
	char getType(){
    	return 'a';
    }
    virtual
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) {};
    virtual
    ~AscalFrame() {};
    void clearStackIfAnotherStatementProceeds()
    {
    	this->index++;
    	bool clear = false;
    	while(!clear && this->exp[index])
    	{
    		if(isalpha(this->exp[index]) || (this->exp[index] >= 48 && this->exp[index] < 58) || Calculator<double>::isOperator(this->exp[index]))
    		{
    			clear = true;
    		}
    		index++;
    	}
    	if(clear)
    	{
            while(!this->initialOperands.isEmpty() || !this->initialOperators.isEmpty())
            {
                this->initialOperands.pop();
                this->initialOperators.pop();
            }
    	}
    }
};
static uint16_t runningNumber = 1;
template <typename t>
class ParamFrame: public AscalFrame<t> {
private:
public:
    ParamFrame(AscalParameters* params, std::map<std::string,Object*>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
    }
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
    {
        if(this->returnPointer)
        {
        	Object *obj = &((*this->localMemory)[std::to_string(runningNumber++)] = Object("",std::to_string(result),""));
            this->returnPointer->getParams()->push_back(obj);
        }
    }
	char getType() override
	{
		return 'p';
	}
    ~ParamFrame(){};
};
template <typename t>
class ParamFrameFunctionPointer: public AscalFrame<t> {
public:
    Object *obj;
    ParamFrameFunctionPointer(AscalParameters* params, std::map<std::string,Object*>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
    }
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
    {
    	if(this->returnPointer && obj)
    	{
    		this->returnPointer->getParams()->push_back(obj);
    	}
        else
        	throw std::string("Error cannot find Object passed as reference param.");

    }

	char getType() override
	{
		return 'z';
	}
    ~ParamFrameFunctionPointer(){};
};
template <typename t>
class FunctionFrame: public AscalFrame<t> {
private:
	std::map<std::string,Object*> paramMem;
	std::map<std::string,Object> localMem;
	AscalParameters param;
public:
    FunctionFrame(AscalParameters* params, std::map<std::string,Object*>* paramMemory, std::map<std::string,Object>* localMemory)
    {
        this->params = &param;
        this->paramMemory = &paramMem;
        this->localMemory = &localMem;
        //bitwise or with the 2^5 bit 32 sets isFunction true
        this->flagRegisters |= 32;
    }
    void returnResult(t result, std::unordered_map<std::string, Object>& globalMemory) override
    {
        if(this->returnPointer)
        {
            this->returnPointer->initialOperands.push_back(result);
        }
    }
	char getType() override
	{
		return 'f';
	}
    ~FunctionFrame(){}
};

#endif /* ASCALFRAME_HPP_ */
