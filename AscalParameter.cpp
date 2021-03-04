/*
 * AscalParameters.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: andrew
 */

#include "AscalParameters.hpp"

AscalParameters::AscalParameters() {
	useCount = 0;
}


std::string AscalParameters::toString()
{
	std::string data;
	for(std::string &s:*this)
	{
		data += s+",";
	}
	return data;
}

AscalParameters& AscalParameters::operator++()
{
	++useCount;
	return *this;
}

AscalParameters& AscalParameters::operator+=(int val)
{
	useCount += val;
	return *this;
}
int AscalParameters::getUseCount()
{
	return useCount;
}

void AscalParameters::resetParamUse()
{
	useCount = 0;
}
