/*
 * stm32f4xx.c
 *
 *  Created on: Dec 20, 2020
 *      Author: vinhta
 */


#include "../Inc/stm32f4xx.h"

/* Define reg base addresses */

RCC_reg* RCC = (RCC_reg *) RCC_base;

FLASH_reg* FLASH = (FLASH_reg *) FLASH_base;

EXTI_reg *EXTI = (EXTI_reg *) EXTI_base;

SYSCFG_reg *SYSCFG = (SYSCFG_reg *) SYSCFG_base;

RTC_reg *RTC = (RTC_reg *) RTC_base;

RNG_reg *RNG = (RNG_reg *) RNG_base;

PWR_reg *PWR = (PWR_reg *) PWR_base;

NVIC_ISER_reg * NVIC_ISER = (NVIC_ISER_reg*) NVIC_ISER_base;
NVIC_ICER_reg * NVIC_ICER = (NVIC_ICER_reg*) NVIC_ICER_base;

GPIOx_reg *GPIOA = (GPIOx_reg *) GPIOA_base;
GPIOx_reg *GPIOB = (GPIOx_reg *) GPIOB_base;
GPIOx_reg *GPIOC = (GPIOx_reg *) GPIOC_base;
GPIOx_reg *GPIOD = (GPIOx_reg *) GPIOD_base;
GPIOx_reg *GPIOI = (GPIOx_reg *) GPIOI_base;
GPIOx_reg *GPIOE = (GPIOx_reg *) GPIOE_base;
GPIOx_reg *GPIOF = (GPIOx_reg *) GPIOF_base;
GPIOx_reg *GPIOG = (GPIOx_reg *) GPIOG_base;
GPIOx_reg *GPIOH = (GPIOx_reg *) GPIOH_base;

I2Cx_reg *I2C1 = (I2Cx_reg *) I2C1_base;
I2Cx_reg *I2C2 = (I2Cx_reg *) I2C2_base;
I2Cx_reg *I2C3 = (I2Cx_reg *) I2C3_base;

TIM1_8_reg *TIM1 = (TIM1_8_reg*) TIM1_base;
TIM1_8_reg *TIM8 = (TIM1_8_reg*) TIM8_base;
TIM2_5_reg *TIM2 = (TIM2_5_reg*) TIM2_base;
TIM2_5_reg *TIM3 = (TIM2_5_reg*) TIM3_base;
TIM2_5_reg *TIM4 = (TIM2_5_reg*) TIM4_base;
TIM2_5_reg *TIM5 = (TIM2_5_reg*) TIM5_base;
TIM6_7_reg *TIM6 = (TIM6_7_reg*) TIM6_base;
TIM6_7_reg *TIM7 = (TIM6_7_reg*) TIM7_base;
TIM9_14_reg *TIM9 = (TIM9_14_reg*) TIM9_base;
TIM9_14_reg *TIM10 = (TIM9_14_reg*) TIM10_base;
TIM9_14_reg *TIM11 = (TIM9_14_reg*) TIM11_base;
TIM9_14_reg *TIM12 = (TIM9_14_reg*) TIM12_base;
TIM9_14_reg *TIM13 = (TIM9_14_reg*) TIM13_base;
TIM9_14_reg *TIM14 = (TIM9_14_reg*) TIM14_base;


// System timer handler
unsigned sysclk_frequency = 15900000;
unsigned apb1_frequency = 15900000;
unsigned apb1_timer = 15900000;
unsigned apb2_frequency = 15900000;
unsigned apb2_timer = 15900000;
unsigned volatile systick_overlap = 0;
unsigned volatile RTC_Cycle = 0;

void TIM5_IRQHandler(void){
	systick_overlap++;
	CLEAR(TIM5->SR, 0, 0x1);
}

void msec_delay(unsigned delay){
	// Delay using RTC clock (4.096KHz)
	//unsigned stop = RTC_Cycle + (4*delay) + (delay*96/1000);  // Off by 0.2207msec/sec
	//while(stop > RTC_Cycle);

	// Delay using internal systick
	unsigned duration = (apb1_timer/1000)*delay;
	//duration += 200;
	unsigned stop = 0;
	if(duration > (0xFFFFFFFF - get_sys_tick())){  // Overflow - high overhead but very rare occasion
		stop = duration - (0xFFFFFFFF - get_sys_tick());	// Only works with 1 overlap max
		while(get_sys_tick() <= 100);		// Wait for counter reset with 100 cycles uncertainty
	} else
		stop = get_sys_tick() + duration;

	while(stop > get_sys_tick());
}

void usec_delay(unsigned delay){
	//if(delay < 20)
		//delay = 20;

	unsigned duration = ((apb1_timer/1000000) * delay); // + (delay*10/111);  // account for 0.9MHz
	duration -= 60;	// Manually measured ~5usec for calculations in 16MHz sysclk

	unsigned stop = 0;
	if(duration > (0xFFFFFFFF - get_sys_tick())){  // Overflow
		stop = duration - (0xFFFFFFFF - get_sys_tick());
		while(get_sys_tick() <= 100);		// Wait for counter reset with 100 cycles uncertainty
	} else
		stop = get_sys_tick() + duration;

	while(stop > get_sys_tick());
}

unsigned get_sys_msec(){
	unsigned tick = TIM5->CNT;
	return systick_overlap*(0xFFFFFFFF/(apb1_timer/1000)) + tick/(apb1_timer/1000);
}

/*unsigned get_sys_tick(){
	return TIM5->CNT;
}

unsigned get_tick_overlap(){
	return systick_overlap;
}*/

// Following function assumes RTC_Cycle is 4.096kHz freq
unsigned get_RTC_msec(){
	return ((RTC_Cycle/1000)*244) + (RTC_Cycle/7120);  // Account for decimal -> only off by 0.175ns/sec!
		// Doing mult before div makes better precision but requires more bits
		// In 32b, it only lasts to Cycle*244=2^32 -> Cycle = 17.6M ~ 4300sec before overflow
		// If doing div first, it will be off by at most whole cycle ~ 0.244msec, but overflows at 2^32sec
}
