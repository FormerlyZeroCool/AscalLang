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
#include "AscalExecutor.hpp"
#include "Object.hpp"
#include "stack.hpp"
#include "StackSegment.hpp"
#include "Calculator.hpp"
#include "string_view.hpp"
#include "MemoryMap.hpp"
#include "HashMap.hpp"

template <typename t>
class AscalFrame {
private:

protected:
    //+1 sets if result flag to true, sp no else cases wil be initally executed
    //+8 sets isDynamicAllocation true, so that the calculateExpression functions is responsible for managin the memory allocated for this frame
    //+64 designates the frame as never run
    uint16_t flagRegisters = 1 + 8 + 64;
    AscalParameters* params;
    Map<string_view,Object*>* paramMemory;
    MemoryMap* localMemory;
    Object *contextObj = nullptr;

public:
    AscalFrame* returnPointer = nullptr;
    uint16_t level = 0;
    uint32_t index = 0;
    uint64_t memoPointer = 0;
    string_view exp;
    StackSegment<t> initialOperands;
    StackSegment<char> initialOperators;
    inline void setObjectReturnRegister(Object *obj)
    {
        //set return flags so auto return does not occur, and upon returning to calling function it is aware an object has been returned
        this->flagRegisters |= 256|512;
        (*this->localMemory)[string_view("_rtn")] = *obj;
    }
    inline Object* getReturnObject()
    {
        this->flagRegisters &= 65535-256-512;
        return &(*this->localMemory)[string_view("_rtn")];
    }
    inline Object* getContext()
    {
        return contextObj;
    }
    inline void setContext(Object *obj)
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
        for(auto &value:*localMemory)
        {
            s<<value.toString()<<"\n";
        }
        return s.str();
    }
    inline AscalFrame<t>*  getReturnPointer()
    {
        return returnPointer;
    }
    //The retrieve data from flag registers functions bitmask all the bits that don't represent the data the user wishes to retreive
    //Then auto cast the byte to a bool
    inline bool ifResultFlag() { return flagRegisters & (uint16_t)1; };
    inline bool ifFlag() { return flagRegisters & (uint16_t)2; };
    inline bool comingfromElse() { return flagRegisters & (uint16_t)4; };
    inline bool isDynamicAllocation() { return flagRegisters & (uint16_t)8; };
    inline bool isRoot() { return flagRegisters & (uint16_t)16; };
    inline bool isFunction() { return flagRegisters & (uint16_t)32; };
    inline bool isFirstRun() { return flagRegisters & (uint16_t)64; };
    inline bool isAugmented() { return flagRegisters & (uint16_t)128; };
    inline bool isReturnObjectFlagSet() { return flagRegisters & (uint16_t)256; };
    inline bool isReturnFlagSet() { return flagRegisters & (uint16_t)512; };
    inline bool isZeroFlagSet() { return flagRegisters & (uint16_t)1024; };
    inline void setIfResultFlag(bool value)
    {
        flagRegisters &= (uint16_t)65535-1;
        flagRegisters ^= (uint16_t)(1*value);
    };
    inline void setIfFlag(bool value)
    {
        flagRegisters &= (uint16_t)65535-2;
        flagRegisters ^= (uint16_t)(2*value);
    };
    inline void setComingfromElse(bool value)
    {
        flagRegisters &= (uint16_t)65535-4;
        flagRegisters ^= (uint16_t)(4*value);
    };
    inline void setIsDynamicAllocation(bool value)
    {
        flagRegisters &= (uint16_t)(65535-8);
        flagRegisters ^= (uint16_t)(8*value);
    };
    inline void setIsRoot(bool value)
    {
        flagRegisters &= (uint16_t)(65535-16);
        flagRegisters ^= (uint16_t)(16*value);
    };
    inline void setIsFirstRun(bool value)
    {
        flagRegisters &= (uint16_t)(65535-64);
        flagRegisters ^= (uint16_t)(64*value);
    };
    inline void setAugmented(bool value)
    {
        flagRegisters &= (uint16_t)(65535-128);
        flagRegisters ^= (uint16_t)(128*value);
    };
    inline void setReturnFlag(bool value)
    {
        flagRegisters &= (uint16_t)(65535-512);
        flagRegisters ^= (uint16_t)(512*value);
    };
    inline void setZeroFlag(bool value)
    {
        flagRegisters &= (uint16_t)(65535-1024);
        flagRegisters ^= (uint16_t)(1024*value);
    };
    inline Map<string_view,Object*>* getParamMemory(){ return paramMemory; };
    inline MemoryMap* getLocalMemory(){ return localMemory; };
    inline AscalParameters* getParams(){ return params; };
    virtual
    char getType(){
        return 'a';
    }
    virtual
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) = 0;
    virtual
    ~AscalFrame() {};
    void clearStackIfAnotherStatementProceeds()
    {
        this->index++;
        bool clear = false;
        while(!clear && this->exp.size() > index)
        {
            if(isalpha(this->exp[index]) || (this->exp[index] >= 48 && this->exp[index] < 58) || Calculator<double>::isOperator(this->exp[index]))
            {
                clear = true;
            }
            index++;
        }
        if(clear)
        {
            while(!this->initialOperands.isEmpty() & !this->initialOperators.isEmpty())
            {
                this->initialOperands.pop();
                this->initialOperators.pop();
            }
            while(!this->initialOperands.isEmpty())
            {
                this->initialOperands.pop();
            }
            while(!this->initialOperators.isEmpty())
            {
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
    ParamFrame(AscalExecutor &runtime, AscalParameters* params, Map<string_view,Object*>* paramMemory, MemoryMap* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    ParamFrame(AscalExecutor &runtime, AscalFrame<double>* base)
    {
        this->params = base->getParams();
        this->paramMemory = base->getParamMemory();
        this->localMemory = base->getLocalMemory();
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) override
    {
        if(this->returnPointer)
        {
            char id[2];
            id[0] = 2;
            id[1] = this->returnPointer->getParams()->size();
            Object &obj = this->returnPointer->getLocalMemory()->insert(string_view(id, 2), string_view(id, 0));
            //std::cout<<"Param frame rtning: "<<result<<"\n";
            obj.setDouble(result);
            this->returnPointer->getParams()->push_back(&obj);
        }
    }
    char getType() override
    {
        return 'p';
    }
    ~ParamFrame(){};
};
template <typename t>
class FunctionSubFrame: public AscalFrame<t> {
private:
public:
    FunctionSubFrame(AscalExecutor &runtime, AscalParameters* params, Map<string_view,Object*>* paramMemory, MemoryMap* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    FunctionSubFrame(AscalExecutor &runtime, AscalFrame<double>* base)
    {
        this->params = base->getParams();
        this->paramMemory = base->getParamMemory();
        this->localMemory = base->getLocalMemory();
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) override
    {
        if(this->returnPointer)
        {
            this->initialOperands.clear();
            this->returnPointer->initialOperands.push_back(result);
            this->initialOperands.resetStart();
        }
    }
    char getType() override
    {
        return 's';
    }
    ~FunctionSubFrame(){};
};
template <typename t>
class ConditionalFrame: public AscalFrame<t> {//before this on stack must be a function frame that is the remainder of the conditionals
    //then the code block frame to be executed if this evalutaes to true, and then this
private:
public:
    ConditionalFrame(AscalExecutor &runtime, AscalParameters* params, Map<string_view,Object*>* paramMemory, MemoryMap* localMemory)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    ConditionalFrame(AscalExecutor &runtime, AscalFrame<double>* base)
    {
        this->params = base->getParams();
        this->paramMemory = base->getParamMemory();
        this->localMemory = base->getLocalMemory();
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) override
    {
        if(this->returnPointer)
        {
            this->initialOperands.clear();
            if(!result)
            {
                AscalFrame<double> *frameToBeRemoved = nullptr;
                executionStack.top(frameToBeRemoved);
                executionStack.pop();
                
                if(frameToBeRemoved && frameToBeRemoved->isDynamicAllocation())
                    executionEnv.deleteFrame(frameToBeRemoved);
            
            }
            else
            {
                AscalFrame<double> *frameToBeExec = nullptr;
                executionStack.top(frameToBeExec);
                executionStack.pop();
                
                AscalFrame<double> *frameToBeRemoved = nullptr;
                executionStack.top(frameToBeRemoved);
                executionStack.pop();
                
                if(frameToBeRemoved && frameToBeRemoved->isDynamicAllocation())
                    executionEnv.deleteFrame(frameToBeRemoved);
                
                executionStack.push(frameToBeExec);
                //remove remainder of unused conditionals, and possible branches put back this branch to top of stack when complete
            }
            this->initialOperands.resetStart();
        }
    }
    char getType() override
    {
        return 'c';
    }
    ~ConditionalFrame(){};
};
template <typename t>
class ParamFrameFunctionPointer: public AscalFrame<t> {
public:
    Object *obj;
    ParamFrameFunctionPointer(AscalExecutor &runtime, AscalParameters* params, Map<string_view,Object*>* paramMemory, MemoryMap* localMemory):
        obj(nullptr)
    {
        this->params = params;
        this->paramMemory = paramMemory;
        this->localMemory = localMemory;
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    ParamFrameFunctionPointer(AscalExecutor &runtime, AscalFrame<double> *base):
        obj(nullptr)
    {
        this->params = base->getParams();
        this->paramMemory = base->getParamMemory();
        this->localMemory = base->getLocalMemory();
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
    }
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) override
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
#include <map>
#include <iostream>
template <typename t>
class FunctionFrame: public AscalFrame<t> {
private:
    Map<string_view,Object*> paramMem;
    MemoryMap localMem;
    AscalParameters param;
public:
    FunctionFrame(AscalExecutor &runtime, MemoryManager &memMan) : localMem(memMan), paramMem(memMan.node_pool)
    {
        param.setMemory(runtime.paramsStack);
        this->params = &param;
        this->paramMemory = &paramMem;
        this->localMemory = &localMem;
        this->initialOperands.setData(runtime.operands);
        this->initialOperators.setData(runtime.operators);
        //bitwise or with the 2^5 bit 32 sets isFunction true
        this->flagRegisters |= 32;
    }
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) override
    {
        if(this->returnPointer)
        {
            this->initialOperands.clear();
            this->returnPointer->initialOperands.push_back(result);
            this->initialOperands.resetStart();
        }
    }
    char getType() override
    {
        return 'f';
    }
    ~FunctionFrame(){}
};

#endif /* ASCALFRAME_HPP_ */
