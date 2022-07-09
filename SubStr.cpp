/*
 * SubStr.cpp
 *
 *  Created on: Feb 23, 2021
 *      Author: andrewrubinstein
 */
#include "SubStr.hpp"

SubStr::SubStr(SubStrSV s)
{
	data = s.data.str();
	start = s.start;
	end = s.end;
}


