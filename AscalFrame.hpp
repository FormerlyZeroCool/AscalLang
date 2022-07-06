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
struct StackDataRecord {
    static inline const uint64_t DOUBLE = 0, OWNED = 1, REFERENCED = 2;
    union {
        Object *obj;
        double number;
    } data;
    uint64_t type = -1;
    StackDataRecord() { data.obj = (nullptr); }
    StackDataRecord(uint8_t type, Object *obj): type(type) { data.obj = (obj); }
    StackDataRecord(uint8_t type, double number): type(type) { data.number = (number); }
    constexpr bool isDouble() const
    {
        return type == DOUBLE;
    }
    constexpr bool isOwned() const
    {
        return type == OWNED;
    }
    constexpr bool isReference() const
    {
        return type == REFERENCED;
    }
    constexpr bool isObject() const
    {
        return isOwned() | isReference();
    }
};
template <typename t>
class AscalFrame {
private:

protected:
    //+1 sets if result flag to true, sp no else cases wil be initally executed
    //+8 sets isDynamicAllocation true, so that the calculateExpression functions is responsible for managin the memory allocated for this frame
    //+64 designates the frame as never run
    //uint16_t flagRegisters = 1 + 8 + 64;

public:
    AscalExecutor &runtime;
    string_view exp;
    uint32_t index = 0;
    StackSegment<double> initialOperands;
    StackSegment<StackDataRecord> localMemory;
    inline Object* getContext()
    {
        return nullptr;
    }
    inline void setContext(Object *obj)
    {
        this->contextObj = obj;
    }
    std::string memToString()
    {
        std::stringstream s;
        return s.str();
    }
    //The retrieve data from flag registers functions bitmask all the bits that don't represent the data the user wishes to retreive
    //Then auto cast the byte to a bool
    /*inline bool ifResultFlag() { return flagRegisters & (uint16_t)1; };
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
    };*/
    inline auto& getLocalMemory(){ return localMemory; };
    
    void returnResult(t result, FlatMap<string_view, Object*>& globalMemory, StackSegment<AscalFrame<double>*> &executionStack, AscalExecutor &executionEnv) 
    {
        if(this->returnPointer)
        {
            this->initialOperands.clear();
            this->returnPointer->initialOperands.push_back(result);
            this->initialOperands.resetStart();
        }
    }
    char getType()
    {
        return 'f';
    }
    AscalFrame(AscalExecutor &runtime): runtime(runtime), initialOperands(runtime.operands), localMemory(runtime.dataStack) {}
    void freeOwnedObjects(){
        for(int i = 0; i < this->localMemory.size(); i++)
        {
            const auto rec = this->localMemory[i];
            if(rec.isOwned())
            {
                runtime.memMan.obj_free(rec.data.obj);
            }
        }
    }
    ~AscalFrame() {
        freeOwnedObjects();
    }
};
static uint16_t runningNumber = 1;

#endif /* ASCALFRAME_HPP_ */
