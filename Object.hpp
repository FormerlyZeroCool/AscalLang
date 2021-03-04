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

class Object {
private:
	std::string instructions;
	MemoryMap objectMap;
	std::vector<Object> objectList;
	std::string toString(uint16_t depth);
    Object *parent = nullptr;
public:
	std::string id;
	std::vector<SubStrSV> params;
	Object* getThis();
	//returns end index of params in string
	void setDouble(long double d);
	long double getDouble() const;
	bool isDouble();
	int setParams(string_view param, uint32_t = 0);
	Object(MemoryManager<Object> &memMan);
	Object(MemoryManager<Object> &memMan, std::string &id);
	Object(MemoryManager<Object> &memMan, std::string &&id);
	Object(MemoryManager<Object> &memMan, std::string &id, std::string &param);
	Object(MemoryManager<Object> &memMan, std::string &id,std::string &expression,std::string &param);
	Object(MemoryManager<Object> &memMan, std::string &&id,std::string &&expression,std::string &param);
	Object(MemoryManager<Object> &memMan, std::string &id,std::string &expression,std::string &&param);
	Object(MemoryManager<Object> &memMan, std::string &id,std::string &&expression,std::string &&param);
	Object(MemoryManager<Object> &memMan, std::string &&id,std::string &&expression,std::string &&param);
	Object(MemoryManager<Object> &memMan, std::string &&id,const std::string &expression,std::string &&param);
	void addInstruction(std::string expression);
	std::string instructionsToFormattedString() const ;
	std::string instructionsToFormattedString(uint16_t depth) const;
	std::string& getInstructions();
	std::string listToString(MemoryMap &memory);
	Object& getMapUnsafe(string_view id);
	Object& operator[](string_view id);
	Object& operator[](size_t index);
	Object& loadChild(Object &data, AscalExecutor &);
	bool isList();
	void clearList();
	void pushList(Object &data);
	void pushList(Object &&data);
	Object& setList(Object &data, size_t index);
	Object splitString(string_view filter, MemoryMap &);
	void loadString(string_view s);
	void printList(MemoryMap &memory);
	size_t getListSize();
	Object& getListElement(size_t index,MemoryMap &memory);
	bool operator==(const Object &o) const;
	bool operator==(Object &o) const;
	Object& operator=(const Object& o);
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

#endif /* OBJECT_HPP_ */
