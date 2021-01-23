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
#include "ObjectKey.hpp"
#include "SubStr.hpp"
#include "string_view.hpp"
#include "ParsingUtil.hpp"
class Object {
private:
	std::string instructions;
	std::map<std::string, Object> objectMap;
	std::vector<Object> objectList;
	std::string toString(uint16_t depth);
    Object *parent = nullptr;
public:
	std::string id;
	std::vector<SubStr> params;
	Object* getThis();
	//returns end index of params in string
	int setParams(std::string &param, uint32_t = 0);
	Object();
	Object(std::string &id,std::string &expression,std::string &param);
	Object(std::string &&id,std::string &&expression,std::string &param);
	Object(std::string &id,std::string &expression,std::string &&param);
	Object(std::string &id,std::string &&expression,std::string &&param);
	Object(std::string &&id,std::string &&expression,std::string &&param);
	Object(std::string &&id,const std::string &expression,std::string &&param);
	void addInstruction(std::string expression);
	std::string instructionsToString();
	std::string instructionsToFormattedString();
	std::string instructionsToFormattedString(uint16_t depth);
	std::string& getInstructions();
	std::string listToString(std::unordered_map<std::string,Object> &memory);
	Object& getMapUnsafe(std::string &id);
	Object& operator[](std::string &id);
	Object& operator[](std::string &&id);
	Object& operator[](size_t index);
	Object& loadChild(Object &data);
	bool isList();
	void clearList();
	void pushList(Object &data);
	void pushList(Object &&data);
	Object& setList(Object &data, size_t index);
	Object splitString(string_view filter, std::unordered_map<std::string,Object> &);
	void loadString(string_view s);
	void printList(std::unordered_map<std::string,Object> &memory);
	size_t getListSize();
	Object& getListElement(size_t index,std::unordered_map<std::string,Object> &memory);
	bool operator==(Object o);
	std::string toString();
	std::string compileInstructions(uint32_t start);
	std::stringstream& compileIf(std::stringstream &instStream, uint32_t &index);
	std::stringstream& compileWhen(std::stringstream &instStream, uint32_t &index);
	virtual ~Object();
};

#endif /* OBJECT_HPP_ */
