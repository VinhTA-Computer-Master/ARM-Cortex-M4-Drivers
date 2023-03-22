/*
 * sysclk_driver.h
 *
 *  Created on: Dec 29, 2022
 *      Author: vinht
 */

#ifndef INC_SYSCLK_DRIVER_H_
#define INC_SYSCLK_DRIVER_H_

#pragma once

#include "stm32f4xx.h"
#include "gpio_driver.h"

#define HSE_Freq 		8000000
#define HSI_Freq 		16000000
#define LSI_Freq		32000		// Fetched to Watchdog and RTC
#define VCO_In_Low 		1000000
#define VCO_In_High		2000000
#define VCO_Out_Low		100000000
#define VCO_Out_High	432000000
#define PLL_Max_Freq	168000000
#define PLL_Q_Max_Freq	48000000
#define AHB_Max_Freq	168000000
#define APB1_Max_Freq	42000000
#define APB2_Max_Freq	84000000

enum SYSCLK_out{
	SYSCLK_HSI = 0,
	SYSCLK_HSE = 1,
	SYSCLK_PLL = 2
};

enum PLL_SRC{
	PLL_SRC_HSI = 0,
	PLL_SRC_HSE
};

enum PLL_Out_Scaler{
	PLL_Out_Div_2 = 0,
	PLL_Out_Div_4,
	PLL_Out_Div_6,
	PLL_Out_Div_8
};


enum MCO1_Config{
	MCO1_SRC_HSI = 0,
	MCO1_SRC_HSE,
	MCO1_SRC_LSE,
	MCO1_SRC_PLL
};

enum MCO2_Config{
	MCO2_SRC_SYSCLK = 0,
	MCO2_SRC_PLLI2S,
	MCO2_SRC_HSE,
	MCO2_SRC_PLL
};

enum MCO_Scaler{
	MCO_Div_1 = 0,
	MCO_Div_2 = 4,
	MCO_Div_3,
	MCO_Div_4,
	MCO_Div_5
};

enum AHB_Scaler{
	AHB_Div_1 = 0,
	AHB_Div_2 = 8,
	AHB_Div_4,
	AHB_Div_8,
	AHB_Div_16,
	AHB_Div_64,
	AHB_Div_128,
	AHB_Div_256,
	AHB_Div_512
};

enum APB_Scaler{
	APB_Div_1 = 0,
	APB_Div_2 = 4,
	APB_Div_4,
	APB_Div_8,
	APB_Div_16
};

typedef struct{
	enum SYSCLK_out output;
	enum PLL_SRC PLL_src;
	enum PLL_Out_Scaler PLL_scaler;
	int VCO_Mult_factor;
	int VCO_Div_factor;
	int PLL_Q;
	enum AHB_Scaler AHB_conf;
	enum APB_Scaler APB1_conf;
	enum APB_Scaler APB2_conf;
} SYSCLK_config;

SYSCLK_config sysclk_conf;

void SYSCLK_Init(SYSCLK_config* config);
unsigned PLL_CLK_Config(enum PLL_SRC src, enum PLL_Out_Scaler P, int N, int M, int Q);
void MCO1_CLK_Config(enum MCO1_Config source, enum MCO_Scaler scale, bool output);  // Only tied to PA8
void MCO2_CLK_Config(enum MCO2_Config source, enum MCO_Scaler scale, bool output);  // Only tied to PC9
void clock_boost(unsigned);


#endif /* INC_SYSCLK_DRIVER_H_ */
