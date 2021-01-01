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

template <typename t>
class AscalFrame {
private:

protected:
    //+1 sets if result flag to true, sp no else cases wil be initally executed
	//+8 sets isDynamicAllocation true, so that the calculateExpression functions is responsible for managin the memory allocated for this frame
	//+64 designates the frame as never run
    uint8_t flagRegisters = 1 + 8 + 64;
    AscalParameters* params;
    std::map<std::string,Object* >* paramMemory;
    std::map<std::string,Object>* localMemory;
    Object *contextObj = nullptr;

public:
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
    uint8_t level = 0;
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
    bool isFirstRun() { return flagRegisters & (uint8_t)64; };
    bool isAugmented() { return flagRegisters & (uint8_t)128; };
    void setIfResultFlag(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)255-1;
        flagRegisters = flagRegisters^(uint8_t)(1*value);
    };
    void setIfFlag(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)255-2;
        flagRegisters = flagRegisters^(uint8_t)(2*value);
    };
    void setComingfromElse(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)255-4;
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
    void setIsFirstRun(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)(255-64);
        flagRegisters = flagRegisters^(uint8_t)(64*value);
    };
    void setAugmented(bool value)
    {
        flagRegisters = flagRegisters&(uint8_t)(255-128);
        flagRegisters = flagRegisters^(uint8_t)(128*value);
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
    void clearStackIfAnotherStatementProceeds(bool (*isOperator)(char))
    {
    	this->index++;
    	bool clear = false;
    	while(!clear && this->exp[index])
    	{
    		if(isalpha(this->exp[index]) || (this->exp[index] >= 48 && this->exp[index] < 58) || isOperator(this->exp[index]))
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
static uint64_t runningNumber = 1;
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
        	Object *obj = &((*this->localMemory)[std::to_string(this->returnPointer->getParams()->size())] = Object("",std::to_string(result),""));
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
        //+32 sets isFunction true
        this->flagRegisters = this->flagRegisters&(255-32);
        this->flagRegisters = this->flagRegisters^32;
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
