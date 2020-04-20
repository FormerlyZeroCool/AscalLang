/*
 * AscalParameters.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: andrew
 */

#include "AscalParameters.h"

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
int AscalParameters::getUseCount()
{
	return useCount;
}

void AscalParameters::resetParamUse()
{
	useCount = 0;
}