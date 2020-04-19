/*
 * AscalParameters.h
 *
 *  Created on: Apr 10, 2020
 *      Author: andrew
 */

#ifndef ASCALPARAMETERS_H_
#define ASCALPARAMETERS_H_
#include <vector>
#include <string>
#include <unordered_map>

class AscalParameters: public std::vector<std::string> {
private:
	int useCount;
public:
	AscalParameters();
	AscalParameters& operator++();
	int getUseCount();
	void resetParamUse();
	std::string toString();
};

#endif /* ASCALPARAMETERS_H_ */
