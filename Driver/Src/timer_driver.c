/*
 * timer_driver.c
 *
 *  Created on: Dec 29, 2022
 *      Author: vinht
 */

#include "../Inc/timer_driver.h"

void TIM_Basic_INIT(TIM_basic_config* config){
	TIM6_7_reg *timer_reg;
	if(config->TIM_6_or_7){
		timer_reg = TIM7;
		TIM7_EN;
	} else {
		timer_reg = TIM6;
		TIM6_EN;
	}


	CLEAR(timer_reg->CR1, 3, 0x1);
	SET(timer_reg->CR1, 3, config->One_Pulse_Mode, 0x1);
	CLEAR(timer_reg->CR1, 1, 0x1);		// Enable UDIS for overflow interrupt and update event
	SET(timer_reg->CR1, 2, 1, 0x1);  	// Set URS bit --> Interrupt only on overflow
	CLEAR(timer_reg->CR1, 7, 0x1);
	SET(timer_reg->CR1, 7, config->Update_Buffer, 0x1);

	CLEAR(timer_reg->DIER, 0, 0x1);
	SET(timer_reg->DIER, 0, config->Interrupt_Enable, 0x1);
	CLEAR(timer_reg->DIER, 8, 0x1);
	SET(timer_reg->DIER, 8, config->DMA_Enable, 0x1);

	CLEAR(timer_reg->PSC, 0, 0xFFFF);
	SET(timer_reg->PSC, 0, config->Prescaler, 0xFFFF);
	CLEAR(timer_reg->ARR, 0, 0xFFFF);
	SET(timer_reg->ARR, 0, config->Reload_value, 0xFFFF);

	if(config->TIM_6_or_7)	 // Config TIM7 overflow interrupt
		// TIM7 has interrupt position 55
		SET(NVIC_ISER->ISER1, (55-32), 1, 0x1);
	else  // Config TIM6 overflow interrupt
		SET(NVIC_ISER->ISER1, (54-32), 1, 0x1);


	SET(timer_reg->CR1, 0, 1, 0x1);  // Fire up counter
}

void TIM_Basic_Disable(TIM_basic_config* config){
	TIM6_7_reg *timer_reg;
	if(config->TIM_6_or_7)
		timer_reg = TIM7;
	else
		timer_reg = TIM6;

	CLEAR(timer_reg->CR1, 0, 0x4F);  // Disable counter and reset to default
	CLEAR(timer_reg->DIER, 0, 0x101);
	CLEAR(timer_reg->PSC, 0, 0xFFFF);
	SET(timer_reg->ARR, 0, 0xFFFF, 0xFFFF);

}

void TIM_IRQHandler(int TIM_x){
	if(TIM_x == 6)
		CLEAR(TIM6->SR, 0, 0x1);
	else if(TIM_x == 7)
		CLEAR(TIM7->SR, 0, 0x1);
}

void TIM_Update_Reload_Value(TIM_basic_config* config, uint16_t value){
	TIM6_7_reg *timer_reg;
	if(config->TIM_6_or_7)
		timer_reg = TIM7;
	else
		timer_reg = TIM6;

	//CLEAR(timer_reg->ARR, 0, 0xFFFF);
	//SET(timer_reg->ARR, 0, value, 0xFFFF);
	timer_reg->ARR = (value);  // ARR value has to be set atomic
	//SET(timer_reg->EGR, 0, 1, 0x1);  // Not necessary to set UG bit
	config->Reload_value = value;
}

void TIM_Update_Scaler(TIM_basic_config* config, uint16_t value){
	TIM6_7_reg *timer_reg;
	if(config->TIM_6_or_7)
		timer_reg = TIM7;
	else
		timer_reg = TIM6;

	//CLEAR(timer_reg->PSC, 0, 0xFFFF);
	//SET(timer_reg->PSC, 0, value, 0xFFFF);
	timer_reg->PSC = (value & 0xFFFF);  // PSC value has to be set at atomic
	//SET(timer_reg->EGR, 0, 1, 0x1);  // Not necessary to set UG bit
	config->Prescaler = value;
}
