/*
 * ObjectKey.cpp
 *
 *  Created on: Oct 28, 2020
 *      Author: andrewrubinstein
 */

#include "ObjectKey.hpp"

ObjectKey::~ObjectKey() {}
ObjectKey::ObjectKey() {}

ObjectKey::ObjectKey(std::string &&operat, std::string &command)
{
	this->first = operat;
	this->second = command;
}
ObjectKey::ObjectKey(std::string &&operat, std::string &&command)
{
	this->first = operat;
	this->second = command;
}
std::string* ObjectKey::getOperation()
{
	return &this->first;
}
std::string* ObjectKey::getCommand()
{
	return &this->second;
}

