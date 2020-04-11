/*
 * Object.h
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <iostream>
#include "AscalParameters.h"
class Object {
public:
	std::string id;
	AscalParameters params;
	std::vector<std::string> instructions;
	//returns end index of params in string
	int setParams(std::string param);
	Object();
	Object(std::string id,std::string expression,std::string param);
	void addInstruction(std::string expression);
	std::string instructionsToString();
	std::vector<std::string>& getInstructions();
	bool operator==(Object o);
	virtual ~Object();
};

#endif /* OBJECT_H_ */
