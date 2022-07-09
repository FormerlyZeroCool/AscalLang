/*
 * AscalParameters.h
 *
 *  Created on: Apr 10, 2020
 *      Author: andrew
 */

#ifndef ASCALPARAMETERS_HPP_
#define ASCALPARAMETERS_HPP_
#include <vector>
#include <unordered_map>
#include "Object.hpp"
#include "StackSegment.hpp"

class AscalParameters: public StackSegment<Object*> {
private:
	uint16_t useCount;
public:
	AscalParameters();
	void setMemory(stack<Object*> &mem)
	{
		this->setData(mem);
	}
	AscalParameters& operator++();
	AscalParameters& operator+=(int);
	int getUseCount();
	void resetParamUse();
	std::string toString();
};

#endif /* ASCALPARAMETERS_HPP_ */
