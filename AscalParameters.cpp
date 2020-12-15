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
