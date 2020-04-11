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


AscalParameters& AscalParameters::operator++()
{
	++useCount;
	return *this;
}
int AscalParameters::getUseCount()
{
	return useCount;
}
