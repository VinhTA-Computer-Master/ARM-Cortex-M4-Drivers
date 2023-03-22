/*
 * gpio_driver.c
 *
 *  Created on: Dec 20, 2020
 *      Author: vinhta
 */


#include "../Inc/gpio_driver.h"


void GPIO_Init(GPIO_Handle_t *GPIOHandle, char port, int pin){
  switch(port){
  case 'A': GPIOHandle->pGPIOx = GPIOA; break;
  case 'B': GPIOHandle->pGPIOx = GPIOB; break;
  case 'C': GPIOHandle->pGPIOx = GPIOC; break;
  case 'D': GPIOHandle->pGPIOx = GPIOD; break;
  case 'E': GPIOHandle->pGPIOx = GPIOE; break;
  case 'F': GPIOHandle->pGPIOx = GPIOF; break;
  case 'G': GPIOHandle->pGPIOx = GPIOG; break;
  case 'H': GPIOHandle->pGPIOx = GPIOH; break;
  case 'I': GPIOHandle->pGPIOx = GPIOI; break;
  }
  GPIOHandle->PinConfig.pin = pin;
  GPIOHandle->PinConfig.port = port;
  GPIO_ClockCtl(GPIOHandle, 1);
}


void GPIO_DeInit_Port(GPIO_Handle_t *GPIOHandle){
	switch(GPIOHandle->PinConfig.port){
	case 'A': SET(RCC->AHB1RSTR, 0, 1, 0x1); break;
	case 'B': SET(RCC->AHB1RSTR, 1, 1, 0x1); break;
	case 'C': SET(RCC->AHB1RSTR, 2, 1, 0x1); break;
	case 'D': SET(RCC->AHB1RSTR, 3, 1, 0x1); break;
	case 'E': SET(RCC->AHB1RSTR, 4, 1, 0x1); break;
	case 'F': SET(RCC->AHB1RSTR, 5, 1, 0x1); break;
	case 'G': SET(RCC->AHB1RSTR, 6, 1, 0x1); break;
	case 'H': SET(RCC->AHB1RSTR, 7, 1, 0x1); break;
	case 'I': SET(RCC->AHB1RSTR, 8, 1, 0x1); break;
	}
	GPIOHandle->pGPIOx = 0;
}


void GPIO_ClockCtl(GPIO_Handle_t *GPIOHandle, bool EN){
	if(EN){
		switch(GPIOHandle->PinConfig.port){
		case 'A': GPIOA_EN; break;
		case 'B': GPIOB_EN; break;
		case 'C': GPIOC_EN; break;
		case 'D': GPIOD_EN; break;
		case 'E': GPIOE_EN; break;
		case 'F': GPIOF_EN; break;
		case 'G': GPIOG_EN; break;
		case 'H': GPIOH_EN; break;
		case 'I': GPIOI_EN; break;
		default: ;
		}
	}
}


void GPIO_SetModer(GPIO_Handle_t *GPIOHandle, unsigned mode){
	if(GPIOHandle->PinConfig.pin > 15) return;
	GPIOHandle->PinConfig.mode = mode;

	CLEAR(GPIOHandle->pGPIOx->MODER, GPIOHandle->PinConfig.pin*2, 0x3);
	SET(GPIOHandle->pGPIOx->MODER, GPIOHandle->PinConfig.pin*2, mode, 0x3);
	//GPIOHandle->pGPIOx->MODER &= ~(0x3 << GPIOHandle->PinConfig->pin*2);
	//GPIOHandle->pGPIOx->MODER |= (0x3 & mode) << (GPIOHandle->PinConfig->pin*2);
}

void GPIO_SetOutputType(GPIO_Handle_t *GPIOHandle, bool mode){
	//if(GPIOHandle->PinConfig.mode != MODER_OUTPUT) return;
	if(GPIOHandle->PinConfig.pin > 15) return;
	GPIOHandle->PinConfig.OPtype = mode;

	CLEAR(GPIOHandle->pGPIOx->OTYPER, GPIOHandle->PinConfig.pin, 0x1);
	SET(GPIOHandle->pGPIOx->OTYPER, GPIOHandle->PinConfig.pin, mode, 0x1);
	//GPIOHandle->pGPIOx->OTYPER &= (0xFFFFFFFF & ((0x1 & mode) << (GPIOHandle->PinConfig.pin)));
}

void GPIO_SetSpeed(GPIO_Handle_t *GPIOHandle, unsigned mode){
	//if(GPIOHandle->PinConfig.mode != MODER_OUTPUT) return;
	if(GPIOHandle->PinConfig.pin > 15) return;
	GPIOHandle->PinConfig.speed = mode;

	CLEAR(GPIOHandle->pGPIOx->OSPEEDR, GPIOHandle->PinConfig.pin*2, 0x3);
	SET(GPIOHandle->pGPIOx->OSPEEDR, GPIOHandle->PinConfig.pin*2, mode, 0x3);
	//GPIOHandle->pGPIOx->OSPEEDR &= (0xFFFFFFFF & ((0x3 & mode) << (GPIOHandle->PinConfig.pin*2)));
}

void GPIO_SetPull(GPIO_Handle_t *GPIOHandle, unsigned mode){
	if(GPIOHandle->PinConfig.pin > 15) return;
	GPIOHandle->PinConfig.pupd = mode;

	CLEAR(GPIOHandle->pGPIOx->PUPDR, GPIOHandle->PinConfig.pin*2, 0x3);
	SET(GPIOHandle->pGPIOx->PUPDR, GPIOHandle->PinConfig.pin*2, mode, 0x3);
	//GPIOHandle->pGPIOx->PUPDR &= (0xFFFFFFFF & ((0x3 & mode) << (GPIOHandle->PinConfig->pin*2)));
}

void GPIO_SetAltFunc(GPIO_Handle_t *GPIOHandle, unsigned func){
	if(GPIOHandle->PinConfig.mode != MODER_ALT_FUNC) return;
	if(GPIOHandle->PinConfig.pin > 15) return;
	if(func > 15) return;
	GPIOHandle->PinConfig.AltFunc = func;

	if(GPIOHandle->PinConfig.pin <= 7){
		CLEAR(GPIOHandle->pGPIOx->AFRL, GPIOHandle->PinConfig.pin*4, 0xF);
		SET(GPIOHandle->pGPIOx->AFRL, GPIOHandle->PinConfig.pin*4, func, 0xF);
		//GPIOHandle->pGPIOx->AFRL &= (0xFFFFFFFF & ((0xF & func) << (GPIOHandle->PinConfig->pin*4)));
	}
	else{
		CLEAR(GPIOHandle->pGPIOx->AFRH, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
		SET(GPIOHandle->pGPIOx->AFRH, (GPIOHandle->PinConfig.pin-8)*4, func, 0xF);
		//GPIOHandle->pGPIOx->AFRH &= (0xFFFFFFFF & ((0xF & func) << ((GPIOHandle->PinConfig->pin-8)*4)));
	}
}

bool GPIO_ReadFromPin(GPIO_Handle_t *GPIOHandle){
	uint16_t port_val = GPIO_ReadFromPort(GPIOHandle);
	return (bool)((port_val >> GPIOHandle->PinConfig.pin) & 0x1);
}


uint16_t GPIO_ReadFromPort(GPIO_Handle_t *GPIOHandle){
	return (GPIOHandle->pGPIOx->IDR & 0xFFFF);
}


void GPIO_WriteToPin(GPIO_Handle_t *GPIOHandle, bool val){
	CLEAR(GPIOHandle->pGPIOx->ODR, GPIOHandle->PinConfig.pin, 0x1);
	SET(GPIOHandle->pGPIOx->ODR, GPIOHandle->PinConfig.pin, val, 0x1);
	//uint16_t port_val = 0xFFFF & (val << GPIOHandle->PinConfig->pin);
	//GPIOHandle->pGPIOx->ODR &= port_val;
}


void GPIO_WriteToPort(GPIO_Handle_t *GPIOHandle, uint16_t val){
	GPIOHandle->pGPIOx->ODR = 0xFFFF & val;
}


void GPIO_TogglePin(GPIO_Handle_t *GPIOHandle){
	TOGGLE(GPIOHandle->pGPIOx->ODR, GPIOHandle->PinConfig.pin, 0x1);
}


void GPIO_IRQConfig(GPIO_Handle_t *GPIOHandle, bool int_evt_mode, int edge_mode){
	if(!(GPIOHandle->pGPIOx)) return;

	SYSCFG_EN;

	switch(GPIOHandle->PinConfig.port){
		case 'A':
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x0, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x0, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x0, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x0, 0xF);
			}
			break;

		case 'B': //CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x1, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x1, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x1, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x1, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x1, 0xF);
			}
			break;

		case 'C': //CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x2, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x2, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x2, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x2, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x2, 0xF);
			}
			break;

		case 'D': //CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x3, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x3, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x3, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x3, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x3, 0xF);
			}
			break;

		case 'E': //CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x4, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x4, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x4, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x4, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x4, 0xF);
			}
			break;

		case 'F': // CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x5, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x5, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x5, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x5, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x5, 0xF);
			}
			break;

		case 'G': // CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x6, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x6, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x6, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x6, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x6, 0xF);
			}
			break;

		case 'H': // CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x7, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x7, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x7, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x7, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x7, 0xF);
			}
			break;

		case 'I': // CLEAR(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0xF); SET(addr, (GPIOHandle->PinConfig.pin-offset)*4, 0x8, 0xF);
			if(GPIOHandle->PinConfig.pin < 4){
				CLEAR(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0xF);
				SET(SYSCFG->CR1, (GPIOHandle->PinConfig.pin)*4, 0x8, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 8){
				CLEAR(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0xF);
				SET(SYSCFG->CR2, (GPIOHandle->PinConfig.pin-4)*4, 0x8, 0xF);
			} else if(GPIOHandle->PinConfig.pin < 12){
				CLEAR(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0xF);
				SET(SYSCFG->CR3, (GPIOHandle->PinConfig.pin-8)*4, 0x8, 0xF);
			} else {
				CLEAR(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0xF);
				SET(SYSCFG->CR4, (GPIOHandle->PinConfig.pin-12)*4, 0x8, 0xF);
			}
			break;
	}

	// Mask EXTI interrupt/event line
	//if(int_evt_mode == GPIO_EXTI_INTERRUPT)
		SET(EXTI->IMR, GPIOHandle->PinConfig.pin, 1, 0x1);
	//else
		//SET(EXTI->EMR, GPIOHandle->PinConfig.pin, 1, 0x1);

	// Set fall/rise/none edge trigger mode
	if(edge_mode == GPIO_EXTI_EDGE_NONE){
		// Reset both
		CLEAR(EXTI->FTSR, GPIOHandle->PinConfig.pin, 0x1);
		CLEAR(EXTI->RTSR, GPIOHandle->PinConfig.pin, 0x1);
	} else if(edge_mode == GPIO_EXTI_EDGE_FALL){
		SET(EXTI->FTSR, GPIOHandle->PinConfig.pin, 1, 0x1);
		CLEAR(EXTI->RTSR, GPIOHandle->PinConfig.pin, 0x1);
	} else {
		SET(EXTI->RTSR, GPIOHandle->PinConfig.pin, 1, 0x1);
		CLEAR(EXTI->FTSR, GPIOHandle->PinConfig.pin, 0x1);
	}


	// Connect to NVIC IRQ channel
	// Refer to Manual Section 12.2 Table 61 for NVIC position #
	// Each position # is controlled by the same bit # in corresponding ISER reg
	if(int_evt_mode == GPIO_EXTI_INTERRUPT){
		int pin = GPIOHandle->PinConfig.pin;
		if(pin == 0)
			NVIC_ISER->ISER0 |= (1 << 6);
		else if(pin == 1)
			NVIC_ISER->ISER0 |= (1 << 7);
		else if(pin == 2)
			NVIC_ISER->ISER0 |= (1 << 8);
		else if(pin == 3)
			NVIC_ISER->ISER0 |= (1 << 9);
		else if(pin == 4)
			NVIC_ISER->ISER0 |= (1 << 10);
		else if(pin < 10)
			NVIC_ISER->ISER0 |= (1 << 23);
		else
			NVIC_ISER->ISER1 |= (1 << (40-32));  // Position 40 controlled by ISER1, offset 32 positions from ISER0
	}
}


void GPIO_IRQHandling(uint8_t pin){
	EXTI->PR |= (1 << pin);  // Clear interrupt flag for line <pin>
}

void GPIO_IRQDisable(GPIO_Handle_t *GPIOHandle, bool disableNVIC){
	int pin = GPIOHandle->PinConfig.pin;

	SET(EXTI->PR, pin, 1, 0x1);
	CLEAR(EXTI->FTSR, pin, 0x1);
	CLEAR(EXTI->RTSR, pin, 0x1);
	CLEAR(EXTI->IMR, pin, 0x1);
	CLEAR(EXTI->EMR, pin, 0x1);

	if(disableNVIC){
	if(pin == 0)
		NVIC_ICER->ICER0 |= (1 << 6);
	else if(pin == 1)
		NVIC_ICER->ICER0 |= (1 << 7);
	else if(pin == 2)
		NVIC_ICER->ICER0 |= (1 << 8);
	else if(pin == 3)
		NVIC_ICER->ICER0 |= (1 << 9);
	else if(pin == 4)
		NVIC_ICER->ICER0 |= (1 << 10);
	else if(pin < 10)
		NVIC_ICER->ICER0 |= (1 << 23);
	else
		NVIC_ICER->ICER1 |= (1 << (40-32));
	}
}

