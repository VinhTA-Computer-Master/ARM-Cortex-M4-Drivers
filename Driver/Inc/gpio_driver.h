/*
 * gpio_driver.h
 *
 *  Created on: Dec 20, 2020
 *      Author: vinhta
 */

#ifndef SRC_GPIO_DRIVER_H_
#define SRC_GPIO_DRIVER_H_

#pragma once

#include "stm32f4xx.h"

typedef struct{
	uint8_t pin;
	uint8_t mode;
	uint8_t speed;
	uint8_t pupd;
	uint8_t OPtype;
	uint8_t AltFunc;
	char port;
} GPIO_PinConfig_t;



typedef struct{
	GPIOx_reg* pGPIOx;   			// Base address of GPIOx port
	GPIO_PinConfig_t PinConfig;		// GPIO Pin configuration
} GPIO_Handle_t;


/************************************************
 *
 * GPIO MODE Macros
 *
 ************************************************/
#define MODER_INPUT   	0x0  // Input to port/pin
#define MODER_OUTPUT  	0x1  // Output from port/pin
#define MODER_ALT_FUNC	0x2  // Refer to STM32F40xx Section 3 Table 9 for function mapping
#define MODER_ANALOG	0x3

#define OTYPER_PUSH_PULL	0
#define OTYPER_OPEN_DRAIN	1

#define OUT_SPEED_8MHz		0
#define OUT_SPEED_50MHz		1
#define OUT_SPEED_100MHz	2
#define OUT_SPEED_180MHz	3

#define PUPDR_NONE		0
#define PUPDR_UP		1
#define PUPDR_DOWN		2

#define GPIO_EXTI_INTERRUPT		0
#define GPIO_EXTI_EVENT			1

#define GPIO_EXTI_EDGE_NONE		0
#define GPIO_EXTI_EDGE_RISE		1
#define GPIO_EXTI_EDGE_FALL		2


/************************************************
 *
 * DRIVER APIs
 *
 ************************************************/

void GPIO_Init(GPIO_Handle_t *GPIOHandle, char port, int pin);	// Assign base addr and enable clock
void GPIO_DeInit_Port(GPIO_Handle_t *GPIOHandle);
void GPIO_ClockCtl(GPIO_Handle_t *GPIOHandle, bool EN);
void GPIO_SetModer(GPIO_Handle_t *GPIOHandle, unsigned mode);   // Set mode to a port and pin, clock handled
void GPIO_SetOutputType(GPIO_Handle_t *GPIOHandle, bool mode);  // Used only with output moder
void GPIO_SetSpeed(GPIO_Handle_t *GPIOHandle, unsigned mode);	// Used only with output moder
void GPIO_SetPull(GPIO_Handle_t *GPIOHandle, unsigned mode);	// Set pull-up/pull-down mode
void GPIO_SetAltFunc(GPIO_Handle_t *GPIOHandle, unsigned func);	// Used only with alternate function moder

bool GPIO_ReadFromPin(GPIO_Handle_t *GPIOHandle);  			// read from individual pin
uint16_t GPIO_ReadFromPort(GPIO_Handle_t *GPIOHandle);  			 			// read from all 16 pins
void GPIO_WriteToPin(GPIO_Handle_t *GPIOHandle, bool val);	// write to a pin
void GPIO_WriteToPort(GPIO_Handle_t *GPIOHandle, uint16_t val);				// write to a port
void GPIO_TogglePin(GPIO_Handle_t *GPIOHandle);					// toggle a pin

// Configure interrupt/event to GPIO, clocks handled, must implement corresponding IRQ handler in driver
void GPIO_IRQConfig(GPIO_Handle_t *GPIOHandle, bool int_evt_mode, int edge_mode);
void GPIO_IRQHandling(uint8_t pin);
void GPIO_IRQDisable(GPIO_Handle_t *GPIOHandle, bool disableNVIC);  // Disable NVIC and reset EXTI for the pin

#endif /* INC_GPIO_DRIVER_H_ */
