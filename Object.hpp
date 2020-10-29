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
#include "AscalParameters.hpp"
#include "ObjectKey.hpp"
class Object {
private:
	std::string instructions;

	std::map<std::string, Object> objectMap;
public:
	std::string id;
	AscalParameters params;
	//returns end index of params in string
	int setParams(std::string &param);
	Object();
	Object(std::string id,std::string expression,std::string param);
	void addInstruction(std::string expression);
	std::string instructionsToString();
	std::string instructionsToFormattedString();
	std::string& getInstructions();
	Object& getChild(std::string &id);
	void pushList(Object &data);
	bool operator==(Object o);
	virtual ~Object();
};

#endif /* OBJECT_HPP_ */
