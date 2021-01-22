/*
 * PRNG.cpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#include "PRNG.hpp"

uint64_t PRNG::hashRand = clock();
uint64_t PRNG::hr2 = 0;
PRNG::PRNG() { }

PRNG::~PRNG() { }

