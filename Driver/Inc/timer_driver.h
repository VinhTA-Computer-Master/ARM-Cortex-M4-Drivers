/*
 * timer_driver.h
 *
 *  Created on: Dec 29, 2022
 *      Author: vinht
 */

#ifndef INC_TIMER_DRIVER_H_
#define INC_TIMER_DRIVER_H_

#include "stm32f4xx.h"

// NOTE: TIM2-7, 12-14 uses APB1 bus, TIM1,8-11 uses APB2 bus
// NOTE: Both UDIS and UIE bits have to be enabled for NVIC trigger

// Only supporting basic timers (TIM6-7) for now
// Upcount only, overflow interrupt supported, update event supported
typedef struct{
	bool TIM_6_or_7;
	bool One_Pulse_Mode;
	bool Update_Buffer;
	bool DMA_Enable;
	bool Interrupt_Enable;
	uint16_t Prescaler;
	uint16_t Reload_value;  // Value to start from after overflow
} TIM_basic_config;


// General timers (TIM2-5, 9-14)
// Supporting slave mode, output capture/compare mode, PWM mode, DMA not supported
typedef struct{
	int TIMx;
	TIM_basic_config basic_config;
	bool count_up_mode;   // True for up, false for down
	bool slave_mode_en;   // True for slave, false for master
	unsigned short CC_channel_en;  // Capture/compare trigger enable for each of 4 channels (0xF for all 4, 0x0 for none)

} TIM_Gen_config;


void TIM_Basic_INIT(TIM_basic_config* config);
void TIM_Basic_Disable(TIM_basic_config* config);
void TIM_IRQHandler(int TIM_x);
void TIM_Update_Scaler(TIM_basic_config* config, uint16_t value);
void TIM_Update_Reload_Value(TIM_basic_config* config, uint16_t value);

#endif /* INC_TIMER_DRIVER_H_ */
