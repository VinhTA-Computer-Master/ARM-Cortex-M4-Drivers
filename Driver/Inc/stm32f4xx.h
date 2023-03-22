/*
 * stm32f4xx.h
 *
 *  Created on: Dec 20, 2020
 *      Author: vinhta
 */

#ifndef SRC_STM32F4XX_H_
#define SRC_STM32F4XX_H_

#pragma once

#include <stdint.h>
#include <stdbool.h>
//#include <gpio_driver.h>

// Memory base addresses
// Refer to Manual Section 2.3 Table 1
// ARM Cortex M4 is byte addressable -> each offset of address points to 8bit of data -> 32b reg takes 4 addr offsets
#define Cortex_base		0xE0000000UL
#define RCC_base 		0x40023800UL
#define EXTI_base		0x40013C00UL
#define SYSCFG_base		0x40013800UL
#define ADCx_base		0x40012000UL
#define DAC_base		0x40007400UL
#define FLASH_base		0x40023C00UL
#define RTC_base		0x40002800UL
#define BKP_base		(RTC_base)
#define RNG_base		0x50060800UL
#define PWR_base		0x40007000UL
#define GPIOA_base		0x40020000UL
#define GPIOB_base		0x40020400UL
#define GPIOC_base		0x40020800UL
#define GPIOD_base		0x40020C00UL
#define GPIOE_base		0x40021000UL
#define GPIOF_base		0x40021400UL
#define GPIOG_base		0x40021800UL
#define GPIOH_base		0x40021C00UL
#define GPIOI_base		0x40022000UL
#define I2C1_base		0x40005400UL
#define I2C2_base		0x40005800UL
#define I2C3_base		0x40005C00UL
#define TIM1_base		0x40010000UL
#define TIM2_base		0x40000000UL
#define TIM3_base		0x40000400UL
#define TIM4_base		0x40000800UL
#define TIM5_base		0x40000C00UL
#define TIM6_base		0x40001000UL
#define TIM7_base		0x40001400UL
#define TIM8_base		0x40010400UL
#define TIM9_base		0x40014000UL
#define TIM10_base		0x40014400UL
#define TIM11_base		0x40014800UL
#define TIM12_base		0x40001800UL
#define TIM13_base		0x40001C00UL
#define TIM14_base		0x40002000UL
#define NVIC_ICTR		0xE000E004UL  // Interrupt controller type register -> See ARM Cortex M4 Manual
#define NVIC_ISER_base	0xE000E100UL
#define NVIC_ICER_base	0xE000E180UL


//////////////////////////////////////

/*  Register definition  */

// RCC Regs
// Refer to Manual Section 7.3
typedef struct{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t reserved;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t reserved1;
	volatile uint32_t reserved2;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t reserved3;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t reserved4;
	volatile uint32_t reserved5;
	volatile uint32_t AHBxLBENR[3];
	volatile uint32_t reserved6;
	volatile uint32_t APBxLBENR[2];
	volatile uint32_t reserved7[2];
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t reserved8[2];
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
} RCC_reg;


// EXTI Regs
// Refer to Manual Section 12.3
typedef struct{
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
} EXTI_reg;

typedef struct{
	volatile uint32_t ISER0;
	volatile uint32_t ISER1;
	volatile uint32_t ISER2;
	volatile uint32_t ISER3;
	volatile uint32_t ISER4;
	volatile uint32_t ISER5;
	volatile uint32_t ISER6;
	volatile uint32_t ISER7;
} NVIC_ISER_reg;

typedef struct{
	volatile uint32_t ICER0;
	volatile uint32_t ICER1;
	volatile uint32_t ICER2;
	volatile uint32_t ICER3;
	volatile uint32_t ICER4;
	volatile uint32_t ICER5;
	volatile uint32_t ICER6;
	volatile uint32_t ICER7;
} NVIC_ICER_reg;


// SYSCFG Regs
// Refer to Manual Section 9.2
typedef struct{
	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t CR4;
	volatile uint32_t CMPCR;
} SYSCFG_reg;



// RTC Regs
// Refer to Manual Section 26.6
typedef struct{
	volatile uint32_t TR;
	volatile uint32_t DR;
	volatile uint32_t CR;
	volatile uint32_t ISR;
	volatile uint32_t PRER;
	volatile uint32_t WUTR;
	volatile uint32_t CALIBR;
	volatile uint32_t ALRMAR;
	volatile uint32_t ALRMBR;
	volatile uint32_t WPR;
	volatile uint32_t SSR;
	volatile uint32_t SHIFTR;
	volatile uint32_t TSTR;
	volatile uint32_t TSSSR;
	volatile uint32_t CALR;
	volatile uint32_t TAFCR;
	volatile uint32_t ALRMASSR;
	volatile uint32_t ALRMBSSR;

} RTC_reg;



// GPIOx Regs
// Refer to Manual Section 8.4
typedef struct{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIOx_reg;


// I2C Regs
// Refer to Manual Section 27.6
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t OAR1;
	volatile uint32_t OAR2;
	volatile uint32_t DR;
	volatile uint32_t SR1;
	volatile uint32_t SR2;
	volatile uint32_t CCR;
	volatile uint32_t TRISE;
	volatile uint32_t FLTR;
} I2Cx_reg;


// TIM1-8 Regs
// Refer to Manual Section 17.4
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1_Out;
	volatile uint32_t CCMR1_In;
	volatile uint32_t CCMR2_Out;
	volatile uint32_t CCMR2_In;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RCR;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t BDTR;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
} TIM1_8_reg;


// TIM2_5 Regs
// Refer to Manual Section 18.4
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RESERVED1;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t RESERVED2;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t TIM_OR;
} TIM2_5_reg;


// TIM9_14 Regs
// Refer to Manual Section 19.4
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t RESERVED1;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RESERVED2;
	volatile uint32_t CCR1;
	volatile uint32_t RESERVED3;
	volatile uint32_t RESERVED4;
	volatile uint32_t RESERVED5;
	volatile uint32_t RESERVED6;
	volatile uint32_t RESERVED7;
	volatile uint32_t RESERVED8;
	volatile uint32_t RESERVED9;
	volatile uint32_t TIM_OR;
} TIM9_14_reg;


// TIM6_7 Regs
// Refer to Manual Section 20.4
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t RESERVED1;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t RESERVED2;
	volatile uint32_t RESERVED3;
	volatile uint32_t RESERVED4;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
} TIM6_7_reg;


// Random Number Generator regs
typedef struct{
	volatile uint32_t CR;
	volatile uint32_t SR;
	volatile uint32_t DR;
} RNG_reg;


// Flash interface register
// Manual section 3.9
typedef struct{
	volatile uint32_t ACR;
	volatile uint32_t KEYR;
	volatile uint32_t OPT_KEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t OPT_CR;
} FLASH_reg;


// Power output regulator interface
// Manual section 5.9
typedef struct{
	volatile uint32_t CR;
	volatile uint32_t CSR;
} PWR_reg;


/* Register base declarations -> addresses assigned in .c file */

RCC_reg *RCC;
EXTI_reg *EXTI;
FLASH_reg *FLASH;
SYSCFG_reg *SYSCFG;
RTC_reg *RTC;
RNG_reg *RNG;
PWR_reg *PWR;
NVIC_ISER_reg *NVIC_ISER;
NVIC_ICER_reg *NVIC_ICER;
GPIOx_reg *GPIOA;
GPIOx_reg *GPIOB;
GPIOx_reg *GPIOC;
GPIOx_reg *GPIOD;
GPIOx_reg *GPIOE;
GPIOx_reg *GPIOF;
GPIOx_reg *GPIOG;
GPIOx_reg *GPIOH;
GPIOx_reg *GPIOI;
I2Cx_reg *I2C1;
I2Cx_reg *I2C2;
I2Cx_reg *I2C3;
TIM1_8_reg *TIM1;
TIM1_8_reg *TIM8;
TIM2_5_reg *TIM2;
TIM2_5_reg *TIM3;
TIM2_5_reg *TIM4;
TIM2_5_reg *TIM5;
TIM6_7_reg *TIM6;
TIM6_7_reg *TIM7;
TIM9_14_reg *TIM9;
TIM9_14_reg *TIM10;
TIM9_14_reg *TIM11;
TIM9_14_reg *TIM12;
TIM9_14_reg *TIM13;
TIM9_14_reg *TIM14;


/* Driver functions */

#define SYSCFG_EN	(RCC->APB2ENR |= (1<<14))

#define RNG_EN		(RCC->AHB2ENR |= (1<<6))

#define PWR_EN		(RCC->APB1ENR |= (1<<28))

#define GPIOA_EN	(RCC->AHB1ENR |= (1<<0))
#define GPIOB_EN 	(RCC->AHB1ENR |= (1<<1))
#define GPIOC_EN 	(RCC->AHB1ENR |= (1<<2))
#define GPIOD_EN 	(RCC->AHB1ENR |= (1<<3))
#define GPIOE_EN 	(RCC->AHB1ENR |= (1<<4))
#define GPIOF_EN 	(RCC->AHB1ENR |= (1<<5))
#define GPIOG_EN 	(RCC->AHB1ENR |= (1<<6))
#define GPIOH_EN 	(RCC->AHB1ENR |= (1<<7))
#define GPIOI_EN 	(RCC->AHB1ENR |= (1<<8))

#define I2C1_EN		(RCC->APB1ENR |= (1<<21))
#define I2C2_EN		(RCC->APB1ENR |= (1<<22))
#define I2C3_EN		(RCC->APB1ENR |= (1<<23))

#define CAN1_EN		(RCC->APB1ENR |= (1<<25))
#define CAN2_EN		(RCC->APB1ENR |= (1<<26))

#define ADC1_EN		(RCC->APB2ENR |= (1<<8))
#define ADC2_EN		(RCC->APB2ENR |= (1<<9))
#define ADC3_EN		(RCC->APB2ENR |= (1<<10))

#define DAC_EN		(RCC->APB1ENR |= (1<<29))

#define TIM1_EN		(RCC->APB2ENR |= (1<<0))
#define TIM2_EN		(RCC->APB1ENR |= (1<<0))
#define TIM3_EN		(RCC->APB1ENR |= (1<<1))
#define TIM4_EN		(RCC->APB1ENR |= (1<<2))
#define TIM5_EN		(RCC->APB1ENR |= (1<<3))
#define TIM6_EN		(RCC->APB1ENR |= (1<<4))
#define TIM7_EN		(RCC->APB1ENR |= (1<<5))
#define TIM8_EN		(RCC->APB2ENR |= (1<<1))
#define TIM9_EN		(RCC->APB2ENR |= (1<<16))
#define TIM10_EN	(RCC->APB2ENR |= (1<<17))
#define TIM11_EN	(RCC->APB2ENR |= (1<<18))
#define TIM12_EN	(RCC->APB1ENR |= (1<<6))
#define TIM13_EN	(RCC->APB1ENR |= (1<<7))
#define TIM14_EN	(RCC->APB1ENR |= (1<<8))


// Bit manipulation macros
#define CLEAR(src, bit, size)		src &= ~(size << bit)
#define SET(src, bit, val, size)	src |= ((val & size) << bit)
#define TOGGLE(src, bit, size)		src ^= (size << bit)
#define READ(src, bit, size)		((src >> bit) & size)


// System timer
//volatile unsigned msec;
void TIM5_IRQHandler(void);
unsigned volatile RTC_Cycle;	// Presume 4.096kHz cycle
unsigned volatile systick_overlap;	// Overlap every 2^32 cycles -> ~268sec/interrupt on 16MHz sysclk or 51.13sec on 168MHz
unsigned sysclk_frequency;
unsigned apb1_frequency;
unsigned apb1_timer;
unsigned apb2_frequency;
unsigned apb2_timer;

void msec_delay(unsigned delay);
void usec_delay(unsigned delay);	// Requires 20usec min for accuracy!
unsigned get_sys_msec();
#define get_sys_tick()	TIM5->CNT
#define get_tick_overlap() systick_overlap
unsigned get_RTC_msec();

#endif /* INC_STM32F4XX_H_ */
