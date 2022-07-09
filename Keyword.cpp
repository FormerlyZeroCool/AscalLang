/*
 * Keyword.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */

#include "Keyword.hpp"
#include "AscalExecutor.hpp"

Keyword::Keyword(AscalExecutor &runtime):
runtime(runtime){}

const std::string Keyword::MAX = std::to_string(std::numeric_limits<double>::max());
Keyword::~Keyword() {}

