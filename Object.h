/*
 * Object.h
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <string>
#include <vector>
#include <iostream>
class Object {
public:
	std::string id;
	std::vector<std::string> params,instructions;
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
