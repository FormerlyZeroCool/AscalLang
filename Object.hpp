/*
 * Object.h
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <iostream>
#include "AscalParameters.hpp"
class Object {
private:
	std::string instructions;
public:
	std::string id;
	AscalParameters params;
	//returns end index of params in string
	int setParams(std::string param);
	Object();
	Object(std::string id,std::string expression,std::string param);
	void addInstruction(std::string expression);
	std::string instructionsToString();
	std::string instructionsToFormattedString();
	std::string& getInstructions();
	bool operator==(Object o);
	virtual ~Object();
};

#endif /* OBJECT_H_ */
