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

class AscalParameters: public std::vector<Object*> {
private:
	uint16_t useCount;
public:
	AscalParameters();
	AscalParameters& operator++();
	AscalParameters& operator+=(int);
	int getUseCount();
	void resetParamUse();
	std::string toString();
};

#endif /* ASCALPARAMETERS_HPP_ */
