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
class Object {
private:
	std::string instructions;
	std::map<std::string, Object> objectMap;
	std::vector<Object> objectList;
	std::string toString(uint16_t depth);
public:
	std::string id;
	std::vector<std::string> params;
	//returns end index of params in string
	int setParams(std::string &param);
	Object();
	Object(std::string id,std::string expression,std::string param);
	void addInstruction(std::string expression);
	std::string instructionsToString();
	std::string instructionsToFormattedString();
	std::string& getInstructions();
	Object getChild(std::string &id);
	bool isList();
	void clearList();
	void pushList(Object &data);
	void pushList(Object &&data);
	void setList(Object &data, size_t index);
	Object splitString(std::string_view filter, std::unordered_map<std::string,Object> &);
	void loadString(std::string_view s);
	void printList(std::unordered_map<std::string,Object> &memory);
	size_t getListSize();
	Object& getListElement(size_t index,std::unordered_map<std::string,Object> &memory);
	bool operator==(Object o);
	std::string toString();
	virtual ~Object();
};

#endif /* OBJECT_HPP_ */
