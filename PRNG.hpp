/*
 * PRNG.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef PRNG_HPP_
#define PRNG_HPP_
#include <inttypes.h>
#include <ctime>
class PRNG {
private:
	static uint64_t lehmer64()
	{
		hr2 += 0xe120fc15;
		uint64_t tmp;
		tmp = (uint64_t)hr2 * 0x4a39b70d;
		uint64_t m1 = (tmp >> 32) ^ tmp;
		tmp = (uint64_t) m1 * 0x12fad5c9;
		uint64_t m2 = (tmp >> 32) ^ tmp;
		return (m2^(m2>>32));

	}
public:
	static uint64_t hashRand, hr2;
	PRNG();
	static long ascalPRNG()
	{
		hashRand ^= lehmer64();
		hashRand ^= hashRand<<30;
		hashRand ^= hashRand>>32;
		hashRand ^= hashRand<<7;
		hashRand ^= hashRand>>45;
		hashRand &= ((uint64_t)1<<31)-1;
		hashRand *= ((hashRand&(1L<<16))!=0)*-1 + ((hashRand&(1L<<16))==0);
		return hashRand;
	}
	virtual ~PRNG();
};

#endif /* PRNG_HPP_ */
