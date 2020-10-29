/*
 * ObjectKey.h
 *
 *  Created on: Oct 28, 2020
 *      Author: andrewrubinstein
 */

#ifndef OBJECTKEY_H_
#define OBJECTKEY_H_
#include <utility>
#include <string>
class ObjectKey: public std::pair<std::string,std::string> {

public:
	ObjectKey();
	ObjectKey(std::string &&operat, std::string &command);
	ObjectKey(std::string &&operat, std::string &&command);
	std::string *getOperation();
	std::string *getCommand();
	virtual ~ObjectKey();
};

#endif /* OBJECTKEY_H_ */
