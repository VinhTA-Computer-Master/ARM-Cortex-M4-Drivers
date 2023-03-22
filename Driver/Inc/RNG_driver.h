/*
 * RNG_driver.h
 *
 *  Created on: Feb 18, 2023
 *      Author: vinht
 */

#ifndef INC_RNG_DRIVER_H_
#define INC_RNG_DRIVER_H_

#include "stm32f4xx.h"

// Return error code
// 0 - no error
// 1 - Seed error after 2 attempts
// 2 - Clock error
uint8_t Random_Number(int * ret, unsigned mod){
	RNG_EN;

	CLEAR(RNG->CR, 3, 0x1);
	SET(RNG->CR, 2, 1, 0x1);
	while(READ(RNG->SR, 0, 0x7) == 0);

	if(READ(RNG->SR, 2, 0x1)){	// Seed error
		CLEAR(RNG->SR, 6, 0x1);
		CLEAR(RNG->CR, 2, 0x1);
		SET(RNG->CR, 2, 1, 0x1);	// Reset RNG

		while(READ(RNG->SR, 0, 0x7) == 0);
		if(READ(RNG->SR, 0, 0x1)){
			ret[0] = (RNG->DR) % mod;
			return 0;
		} else
			return 1;
	}

	if(READ(RNG->SR, 1, 0x1)){
		CLEAR(RNG->SR, 5, 0x1);
		return 2;
	}

	ret[0] = (RNG->DR) % mod;
	return 0;
}

#endif /* INC_RNG_DRIVER_H_ */
