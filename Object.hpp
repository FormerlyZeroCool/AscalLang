/*
 * Object.h
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include "SubStr.hpp"
#include "string_view.hpp"
#include "string_view.hpp"
#include "ObjConstants.hpp"
#include "MemoryMap.hpp"
#include "HashMap.hpp"
#include "Calculator.hpp"
class AscalExecutor;
union member {
    Object *ptr;
    double number;
    uint64_t asInt;
    unsigned char dna[sizeof(double)];
};
class CompilationContext;
struct Record {
    member &data;
    Record(unsigned char *ptr): data(*const_cast<member*>(reinterpret_cast<const member*>(ptr)))
    {
    }
    Record(const unsigned char *ptr): data(*const_cast<member*>(reinterpret_cast<const member*>(ptr)))
    {
    }
    bool isDouble()
    {
        return data.asInt == 1;
    }
    bool isObj()
    {
        return data.asInt == 3;
    }
    bool isStr()
    {
        return data.asInt == 4;
    }
    bool isList()
    {
        return data.asInt == 5;
    }
    //list 5
};
template < class T >
inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
    for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << *ii;
    }
    return os;
}
class Object {
private:
    void *inp = nullptr, *idp = nullptr;
    uint32_t instructionBufferSizeId = 0;
    uint16_t IdBufferSizeId = 0;
    uint16_t flagRegisters = 0;
    uint32_t listSize = 0;
    MemoryMap objectMap;
    Object *parent = nullptr;
    string_view instructions;
    double doubleValue;
    void extracted(const string_view &exp, const string_view &id);
    
    inline void loadData(string_view id, string_view exp);
    void loadInstructions(string_view exp);
    void loadId(string_view id);
    void deallocateMemory(void *ptr, const size_t bufSize, void *idptr, const uint32_t idbufSize);
    void deallocateInstructions(void *ptr, const size_t bufSize);
    void deallocateId(void *ptr, const size_t bufSize);
    std::string toString(uint16_t depth);
public:
    static const uint8_t initialOffset;
    //sizeID codes
    string_view id;
    string_view& getId() { return id; }
    void copyToId(string_view);
    void copyToInstructions(string_view);
    template <typename PLAIN_OLD_OBJECT>//Must be a plain ole object!
    string_view append(const PLAIN_OLD_OBJECT value) {
        std::cout<<"remaining bytes in buffer: "<<(int)(this->instructionBufferSizeId - this->instructions.length())<<"\n";
        if((int)(this->instructionBufferSizeId - this->instructions.length()) <= sizeof(value))
        {
            this->resizeInstructionsCopyDealloc();
            return append(value);
        }
        else
        {
            memcpy(&this->instructions[instructions.size()], &value, sizeof(value));
            instructions.resize(sizeof(value) + instructions.size());
            return this->instructions.substr(this->instructions.size() - sizeof(PLAIN_OLD_OBJECT), sizeof(PLAIN_OLD_OBJECT));
        }
    }
    void resizeInstructionsCopyDealloc()
    {
        void *ptr = this->inp;
        const uint32_t bufSize = this->instructionBufferSizeId;
        //sets the id pointer to be the same as the front of the newly allocated block
        const auto instructionsSize = instructions.size();
        this->resizeInstructions(this->instructionBufferSizeId*2);
        memcpy(&this->instructions[0], ptr, bufSize);
        instructions.resize(instructionsSize);
        this->deallocateInstructions(ptr, bufSize);
    }
    std::vector<SubStrSV> params;
    Object* getThis();
    //returns end index of params in string
    void setObject() {
        this->flagRegisters &= (1<<16) - 3;
        this->flagRegisters |= 1;
    }
    inline void setDouble(double d);
    inline void setDoubleAtIndex(uint32_t index, double d)
    {
        if(index < this->getListSize())
        {
            memcpy(&this->instructions[initialOffset + index*sizeof(double)], &d, sizeof(double));
        }
        else
            throw std::string("Error invalid index: "+std::to_string(index));
    }
    inline double getDouble() const;
    inline bool isDouble();
    double getDoubleAtIndex(uint32_t index)
    {
        if(index < this->getListSize())
        {
            double result = 0;
            memcpy(&result, &this->instructions[initialOffset + index*sizeof(double)], sizeof(double));
            return result;
        }
        else
            throw std::string("Error invalid index: "+std::to_string(index));
    }
    inline Object& getObjectAtIndex(uint32_t index) const;
    int setParams(string_view param, uint32_t = 0);
    void resizeInstructions(uint32_t);
    void clone(const Object &o);
	Object& operator=(const Object &m);
    Object(const Object&);
    Object(MemoryManager &memMan);
    Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &param);
    Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &param);
    Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &&param);
    Object(MemoryManager &memMan, std::string &id,std::string &&expression,std::string &&param);
    Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &&param);
    Object(MemoryManager &memMan, std::string &&id,const std::string &expression,std::string &&param);
    Object(MemoryManager &memMan, string_view id);
    Object(MemoryManager &memMan, string_view id, string_view exp);
    void setParent(Object *parent);
    std::string instructionsToFormattedString() const ;
    std::string instructionsToFormattedString(uint16_t depth) const;
    inline const string_view& getInstructions() { return  instructions; }
    std::string listToString(FlatMap<string_view, Object*> &memory);
    Object& getMapUnsafe(string_view id);
    Object& operator[](string_view id);
    Object& operator[](size_t index);
    Object& loadChild(Object &data, AscalExecutor &);
    bool isList();
    bool isDoubleList();
    bool isObjList();
    inline void clearList();
    void pushList(Object &data);
    void pushList(Object &&data);
    void pushList(double data);
    void eraseList(long index);
    void shrinkInstructions(uint32_t newSize)
    {
        if(this->instructions.size() > newSize)
            this->instructions.resize(newSize);
    }
    Object& setList(Object &data, size_t index);
    Object splitString(string_view filter, FlatMap<string_view, Object*> &);
    Object subString(uint_fast64_t start, uint_fast64_t length, FlatMap<string_view, Object*> &);
    void loadString(string_view s);
    void printList(FlatMap<string_view, Object*> &memory);
    size_t getListSize() const;
    Object* getListElement(size_t index,FlatMap<string_view, Object*> &memory);
    bool operator==(const Object &o) const;
    bool operator==(Object &o) const;
    Object& copyExceptID(const Object& o);
    std::string toString();
    void compileTokens(CompilationContext &ctx, uint32_t end);
    void compileInstructions(AscalExecutor &runtime);
    void compileInstructions(string_view, AscalExecutor &runtime);
    template <typename string>
    void LexCodeAndCompile(string &&s, AscalExecutor &runtime, CompilationContext &ctx)
    {
        this->LexCodeAndCompile(s, runtime, ctx);
    }
    void LexCodeAndCompile(AscalExecutor &runtime, CompilationContext &ctx);
    void compileParams(string_view s, AscalExecutor &runtime, CompilationContext &ctx);
    virtual ~Object();
};

bool Object::isDouble()
{
    return (this->flagRegisters & 4);
}
void Object::setDouble(double d)
{
    this->flagRegisters = 4;
    this->doubleValue = d;
}
double Object::getDouble() const
{
    return doubleValue;
}
Object& Object::getObjectAtIndex(uint32_t index) const
{
    Object *obj = nullptr;
    memcpy(&obj, &this->instructions[initialOffset+index*(sizeof(uint64_t))], sizeof(Object*));
    return *obj;
}
void Object::loadData(string_view id, string_view exp)
{
    loadId(id);
    loadInstructions(exp);
}
void Object::clearList()
{
    if(this->inp && this->isObjList())
    {
        uint64_t index = 0;
        for(uint32_t i = 0; i < this->getListSize(); i++)
        {
            Object *obj = nullptr;
            memcpy(&obj, &this->instructions[initialOffset+i*(sizeof(Object*))], sizeof(Object*));
            objectMap.getMemMan().obj_free(obj);
        }
    }
    
    this->listSize = 0;
    
}
#endif /* OBJECT_HPP_ */
