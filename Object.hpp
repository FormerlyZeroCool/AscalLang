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
#include "ParsingUtil.hpp"
#include "MemoryMap.hpp"

union member {
    Object **ptr;
    double *number;
    uint64_t *asInt;
    unsigned char *dna;
};
struct Record {
    member data;
    unsigned char &byteCode;
    Record(unsigned char *ptr): byteCode(*ptr)
    {
        data.dna = &ptr[1];
    }
    Record(const unsigned char *ptr): byteCode(*(unsigned char*)ptr)
    {
        data.dna = (unsigned char*) &ptr[1];
    }
    bool isDouble()
    {
        return byteCode == 1;
    }
    bool isObj()
    {
        return byteCode == 3;
    }
    bool isStr()
    {
        return byteCode == 4;
    }
    bool isList()
    {
        return byteCode == 5;
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
    static const uint32_t SMALL_EXP, MEDIUM_EXP, LARGE_EXP, VERYLARGE_EXP, MALLOC_EXP, SMALL_ID, LARGE_ID, MALLOC_ID;
    string_view id;
    string_view getId() { return id; }
    void copyToId(string_view);
    void copyToInstructions(string_view);
    std::vector<SubStrSV> params;
    Object* getThis();
    //returns end index of params in string
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
    inline Object& getObjectAtIndex(uint32_t index);
    int setParams(string_view param, uint32_t = 0);
    void resizeInstructions(uint32_t);
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
    std::string listToString(MemoryMap &memory);
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
    Object& setList(Object &data, size_t index);
    Object splitString(string_view filter, MemoryMap &);
    void loadString(string_view s);
    void printList(MemoryMap &memory);
    size_t getListSize();
    Object& getListElement(size_t index,MemoryMap &memory);
    bool operator==(const Object &o) const;
    bool operator==(Object &o) const;
    Object& operator=(const Object& o);
    Object& copyExceptID(const Object& o);
    std::string toString();
    void compileInstructions();
    template <typename string>
    std::string compileInstructions(string &s, uint32_t start);
    template <typename string>
    std::stringstream& compileIf(string &s,std::stringstream &instStream, uint32_t &index);
    template <typename string>
    std::stringstream& compileWhen(string &s, std::stringstream &instStream, uint32_t &index);
    virtual ~Object();
};

bool Object::isDouble()
{
    return this->instructions[0] == 1;
}
void Object::setDouble(double d)
{
    this->instructions[0] = '\1';
    this->instructions.resize(sizeof(double)+initialOffset);
    memcpy(&this->instructions[initialOffset], &d, sizeof(double));
}
double Object::getDouble() const
{
    double number = 0;
    memcpy(&number, &this->instructions[initialOffset], sizeof(uint64_t));
    return number;
}
Object& Object::getObjectAtIndex(uint32_t index)
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
    if(this->isObjList())
    {
        uint64_t index = 0;
        for(uint32_t i = 0; i < this->getListSize(); i++)
        {
            Object *obj = nullptr;
            memcpy(&obj, &this->instructions[initialOffset+i*(sizeof(Object*))], sizeof(Object*));
            objectMap.getMemMan().obj_free(obj);
        }
    }
}
#endif /* OBJECT_HPP_ */
