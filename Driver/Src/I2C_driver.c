/*
 * I2C_driver.c
 *
 *  Created on: Jan 16, 2023
 *      Author: vinht
 */

#include <stdlib.h>
#include "../Inc/I2C_driver.h"
#include "../Inc/stm32f4xx.h"

volatile I2C_Config_t I2C1_conf = {true, true, false, false, 0, 0, 1, 16, 0, 0, NULL, NULL, NULL, NULL, NULL, false};
volatile I2C_Config_t I2C2_conf = {true, true, false, false, 0, 0, 2, 16, 0, 0, NULL, NULL, NULL, NULL, NULL, false};
volatile I2C_Config_t I2C3_conf = {true, true, false, false, 0, 0, 3, 16, 0, 0, NULL, NULL, NULL, NULL, NULL, false};

// Only for LCD2004_App
extern GPIO_Handle_t PD13;

void I2C_Init(volatile I2C_Config_t *conf){
	I2Cx_reg *I2C_reg;

	// Init GPIO pins
	GPIO_Handle_t SCL_Pin, SDA_Pin;
	if(conf->I2C_num == 1){
		if(I2C1_conf.isInit)
			return;
		I2C1_conf.isInit = true;

		if(conf->SCL_alt_pin == 0)
			GPIO_Init(&SCL_Pin, 'B', 6);
		else
			GPIO_Init(&SCL_Pin, 'B', 8);

		if(conf->SDA_alt_pin == 0)
			GPIO_Init(&SDA_Pin, 'B', 7);
		else
			GPIO_Init(&SDA_Pin, 'B', 9);

		I2C1_EN;
		I2C_reg = I2C1;
		conf->reg = I2C1;

	} else if(conf->I2C_num == 2){
		if(I2C2_conf.isInit)
			return;
		I2C2_conf.isInit = true;

		if(conf->SCL_alt_pin == 0)
			GPIO_Init(&SCL_Pin, 'B', 10);
		else if(conf->SCL_alt_pin == 1)
			GPIO_Init(&SCL_Pin, 'F', 1);
		else
			GPIO_Init(&SCL_Pin, 'H', 4);

		if(conf->SDA_alt_pin == 0)
			GPIO_Init(&SDA_Pin, 'B', 11);
		else if(conf->SDA_alt_pin == 1)
			GPIO_Init(&SDA_Pin, 'F', 0);
		else
			GPIO_Init(&SDA_Pin, 'H', 5);

		I2C2_EN;
		I2C_reg = I2C2;
		conf->reg = I2C2;

	} else if(conf->I2C_num == 3){
		if(I2C3_conf.isInit)
			return;
		I2C3_conf.isInit = true;

		if(conf->SCL_alt_pin == 0)
			GPIO_Init(&SCL_Pin, 'H', 7);
		else
			GPIO_Init(&SCL_Pin, 'A', 8);

		if(conf->SDA_alt_pin == 0)
			GPIO_Init(&SDA_Pin, 'H', 8);
		else
			GPIO_Init(&SDA_Pin, 'C', 9);

		I2C3_EN;
		I2C_reg = I2C3;
		conf->reg = I2C3;

	} else
		return;

	conf->bus_freq = apb1_frequency / 1000000;

	GPIO_SetModer(&SCL_Pin, MODER_ALT_FUNC);
	GPIO_SetModer(&SDA_Pin, MODER_ALT_FUNC);
	GPIO_SetAltFunc(&SCL_Pin, 0x4);
	GPIO_SetAltFunc(&SDA_Pin, 0x4);
	GPIO_SetOutputType(&SCL_Pin, OTYPER_OPEN_DRAIN);
	GPIO_SetOutputType(&SDA_Pin, OTYPER_OPEN_DRAIN);
	GPIO_SetPull(&SCL_Pin, PUPDR_UP);
	GPIO_SetPull(&SDA_Pin, PUPDR_UP);
	GPIO_SetSpeed(&SCL_Pin, OUT_SPEED_50MHz);
	GPIO_SetSpeed(&SDA_Pin, OUT_SPEED_50MHz);


	// Set peripheral enable (pin exclusive use for I2C)
	SET(I2C_reg->CR1, 0, 1, 0x1);

	// Set interrupts
	SET(I2C_reg->CR2, 10, 1, 0x1);  // En buffer int
	SET(I2C_reg->CR2, 9, 1, 0x1);	// En event int
	SET(I2C_reg->CR2, 8, conf->error_handler, 0x1);  // Error handler interrupt

	//if(conf->error_handler_callback == NULL)
		//CLEAR(I2C_reg->CR2, 8, 0x1);  // Disable interrupt if error callback is NULL

	// Set bus clk frequency
	if((conf->bus_freq) < 2 || (conf->bus_freq) > 50)
		return;
	CLEAR(I2C_reg->CR2, 0, 0x3F);
	SET(I2C_reg->CR2, 0, (conf->bus_freq), 0x3F);

	CLEAR(I2C_reg->OAR1, 0, 0x3FF);
	SET(I2C_reg->OAR1, 15, !(conf->short_address_bit), 0x1);
	if(conf->short_address_bit)
		SET(I2C_reg->OAR1, 1, conf->ADDR, 0x7F);
	else
		SET(I2C_reg->OAR1, 0, conf->ADDR, 0x3FF);

	if(conf->dual_address){
		SET(I2C_reg->OAR2, 0, 1, 0x1);
		CLEAR(I2C_reg->OAR2, 1, 0x7F);
		SET(I2C_reg->OAR2, 1, conf->ADDR2, 0x7F);
	}

	// Set clock control
	// CCR = Thigh / Tpclk1
	// Thigh = 200kHz (SM), 800kHz(FM), 50% duty cycle default, 100kHz (SM)/400kHz(FM) default
	//   --> Thigh = 5000ns (SM), 1250ns (FM)
	// Tpclk1 = 1e9/bus_freq (ns)
	//   --> CCR = Thigh * bus_freq / 1000  (Minimal round/error calc)
	unsigned Thigh = (conf->fast_mode)? 1250:5000;
	unsigned CCR = (Thigh * conf->bus_freq)/1000;
	SET(I2C_reg->CCR, 15, conf->fast_mode, 0x1);
	CLEAR(I2C_reg->CCR, 0, 0xFFF);
	SET(I2C_reg->CCR, 0, CCR, 0xFFF);
	// Trise = 1000ns max (SM), 250ns max (FM)
	// TRISE reg = Trise/Tpclk1 + 1
	// Tpclk1 = 1e9/(bus_freq*1e6) = 1e3/bus_freq (ns)
	//   --> TRISE reg = (Trise * bus_freq)/1000 +1 (Minimal round/error calc)
	unsigned Trise = (conf->fast_mode)? 250:1000;
	CLEAR(I2C_reg->TRISE, 0, 0x3F);
	SET(I2C_reg->TRISE, 0, (((Trise * conf->bus_freq)/1000) + 1), 0x3F);

	// Enable interrupt line
	if(conf->I2C_num == 1){
		SET(NVIC_ISER->ISER0, 31, 1, 0x1);
		SET(NVIC_ISER->ISER1, (32-32), conf->error_handler, 0x1);
	} else if(conf->I2C_num == 2){
		SET(NVIC_ISER->ISER1, (33-32), 1, 0x1);
		SET(NVIC_ISER->ISER1, (34-32), conf->error_handler, 0x1);
	} else {
		SET(NVIC_ISER->ISER2, (72-64), 1, 0x1);
		SET(NVIC_ISER->ISER2, (73-64), conf->error_handler, 0x1);
	}

	if(conf->slave_rx_callback != NULL || conf->slave_tx_callback != NULL || conf->ADDR != 0)
		SET(I2C_reg->CR1, 10, 1, 0x1);  // Enable ACK in slave mode

	// Enable I2C module
	SET(I2C_reg->CR1, 0, 1, 0x1);
}


void I2C_Reset(volatile I2C_Config_t *conf){
	SET(conf->reg->CR1, 15, 1, 0x1);
	CLEAR(conf->reg->CR1, 15, 0x1);
}


bool I2C_SendData(volatile I2C_Config_t *conf, unsigned char addr, unsigned char *data, unsigned num_bytes){
	// Only for LCD2004_App
	GPIO_WriteToPin(&PD13, 1);

	int stop = get_sys_msec() + 1000;
	while(stop > get_sys_msec() && READ(conf->reg->SR2, 1, 0x1));  // Wait on BUSY flag for 100msec
	if(READ(conf->reg->SR2, 1, 0x1))
		return false;

	conf->status.byte_num = 0;
	conf->status.data = data;  // Data pointer
	conf->status.size = num_bytes;
	conf->status.addr = (addr & 0x7F) << 1;
	if(!conf->short_address_bit){
		conf->status.header = 0xF0 | ((addr & 0x300) >> 8);  // Header for Tx, IRQ should add 1 for Rx!
		conf->status.Sr = false;
	}

	conf->reg->DR = 0;

	// Enable ACK
	SET(conf->reg->CR1, 10, 1, 0x1);

	// Gen. start condition
	//CLEAR(conf->reg->CR1, 9, 0x1);	// Temporary clear of interrupt
	SET(conf->reg->CR1, 8, 1, 0x1);

	// Wait for start bit and send addr here
	/*if(conf->short_address_bit){
		while(!READ(conf->reg->SR1, 0, 0x1));
		conf->reg->DR = conf->status.addr;
	} else {
		while(!READ(conf->reg->SR1, 0, 0x1));
		conf->reg->DR = conf->status.header;
		conf->status.Sr = true;
	}
	SET(conf->reg->CR1, 9, 1, 0x1);*/

	return true;
}

bool I2C_RequestData(volatile I2C_Config_t *conf, unsigned char addr, unsigned char *data, unsigned num_bytes){
	unsigned stop = get_sys_msec() + 100;
	while(stop > get_sys_msec() && READ(conf->reg->SR2, 1, 0x1));  // Wait on BUSY flag for 100msec
	if(READ(conf->reg->SR2, 1, 0x1))
		return false;

	conf->status.byte_num = 0;
	conf->status.data = data;  // Data pointer
	conf->status.size = num_bytes;
	conf->status.addr = ((addr & 0x7F) << 1) + 1;
	if(!conf->short_address_bit){
		conf->status.header = 0xF0 | ((addr & 0x300) >> 7);
		conf->status.Sr = false;
	}

	conf->reg->DR = 0;

	// Enable ACK
	SET(conf->reg->CR1, 10, 1, 0x1);

	// Generate start condition
	SET(conf->reg->CR1, 8, 1, 0x1);

	// Wait for start bit and send addr here
	/*conf->reg->DR = conf->status.addr;
	if(conf->short_address_bit){
		while(!READ(conf->reg->SR1, 0, 0x1));
		conf->reg->DR = conf->status.addr;
	} else {
		while(!READ(conf->reg->SR1, 0, 0x1));
		conf->reg->DR = conf->status.header;
		conf->status.Sr = true;
	}*/

	return true;
}

bool I2C_isBusy(volatile I2C_Config_t *conf){
	return READ(conf->reg->SR2, 1, 0x1);
}

void I2C1_EV_IRQHandler(void){
	// First detect slave/master mode
	if(READ(I2C1_conf.reg->SR2, 0, 0x1)){  // Master mode
		// Detect event num
		if(READ(I2C1_conf.reg->SR1, 0, 0x1)){  // EV5
			if(I2C1_conf.short_address_bit)  	// 8b mode, send addr
				I2C1_conf.reg->DR = I2C1_conf.status.addr & 0xFF;
			else if(!I2C1_conf.status.Sr){	// 10b mode, send header first
				I2C1_conf.reg->DR = I2C1_conf.status.header;  // Tx header first
				I2C1_conf.status.Sr = true;
			} else
				I2C1_conf.reg->DR = I2C1_conf.status.header + 1;  // Then Rx header in 2nd EV5
		}

		if(READ(I2C1_conf.reg->SR1, 1, 0x1)){  // EV6
			if(I2C1_conf.status.size == 1)	 	// In case of 1byte reception, have to clear ACK bit
				CLEAR(I2C1_conf.reg->CR1, 10, 0x1);  // Have to clear ACK bit before clearing ADDR flag

			if(!READ(I2C1_conf.reg->SR2, 2, 0x1) && I2C1_conf.status.Sr){ 	// Read SR2 to clear ADDR flag and check for Rx mode and 1st EV6
				SET(I2C1_conf.reg->CR1, 8, 1, 0x1);  	// Issue repeated start on Rx mode
				I2C1_conf.status.Sr = false;  // Prevent future Sr
			}
		}

		if(READ(I2C1_conf.reg->SR1, 7, 0x1)){ 	  // TxE = 1 -> transmitting, write byte to DR
			if(I2C1_conf.status.byte_num == I2C1_conf.status.size){ // Last byte to Tx
				while(!READ(I2C1_conf.reg->SR1, 2, 0x1));  // Wait till BTF flag is set (byte transmitted)
				SET(I2C1_conf.reg->CR1, 9, 1, 0x1);		// Program stop condition
				//CLEAR(I2C1_conf.reg->CR1, 10, 0x1);		// Disable ACK
				(*(I2C1_conf.master_tx_callback))(1);
				return;
			}
			else
				I2C1_conf.reg->DR = I2C1_conf.status.data[I2C1_conf.status.byte_num++];

		}

		if(READ(I2C1_conf.reg->SR1, 6, 0x1)){  // RxNE = 1 -> receiving, read byte from DR
			I2C1_conf.status.data[I2C1_conf.status.byte_num++] = I2C1_conf.reg->DR & 0xFF;  // Read before issuing NACK

			if(I2C1_conf.status.byte_num == I2C1_conf.status.size){ // 2nd last byte to Rx
				CLEAR(I2C1_conf.reg->CR1, 10, 0x1);   // Disable ACK
				SET(I2C1_conf.reg->CR1, 9, 1, 0x1);   // Program stop condition
				(*(I2C1_conf.master_rx_callback))(1);
				return;
			}
		}

		if(READ(I2C1_conf.reg->SR1, 3, 0x1))  // EV9 - 10b mode only
			I2C1_conf.reg->DR = I2C1_conf.status.addr & 0xFF;

	} else {  // Slave mode
		if(READ(I2C1_conf.reg->SR1, 1, 0x1)){  // EV1, address matched
			I2C1_conf.status.byte_num = 0;  // Init byte num
			I2C1_conf.status.OAR2_matched = READ(I2C1_conf.reg->SR2, 7, 0x1);
			I2C1_conf.status.TX_mode = READ(I2C1_conf.reg->SR2, 2, 0x1);	// TRA = 1 -> slave Tx mode
		}

		if(READ(I2C1_conf.reg->SR1, 7, 0x1))  // EV3 - slave Tx mode
			I2C1_conf.reg->DR = (*(I2C1_conf.slave_tx_callback))(1, I2C1_conf.status.OAR2_matched, I2C1_conf.status.byte_num++) & 0xFF;
		else if(READ(I2C1_conf.reg->SR1, 6, 0x1))  // EV2 - slave Rx mode
			(*(I2C1_conf.slave_rx_callback))(1, I2C1_conf.status.OAR2_matched, I2C1_conf.reg->DR, I2C1_conf.status.byte_num++, false);

		if(READ(I2C1_conf.reg->SR1, 10, 0x1))  // EV3-2 - No ACK received
			CLEAR(I2C1_conf.reg->SR1, 10, 0x1);

		if(READ(I2C1_conf.reg->SR1, 4, 0x1)){   // EV4 - Stop condition detected
			SET(I2C1_conf.reg->CR1, 8, 0x3, 0x3);
			(*(I2C1_conf.slave_rx_callback))(1, I2C1_conf.status.OAR2_matched, 0, I2C1_conf.status.byte_num, true);
		}
	}
}

void I2C1_ER_IRQHandler(void){
	if(I2C1_conf.error_handler_callback != NULL){
		if(READ(I2C1_conf.reg->SR1, 8, 0x1)){
			(*(I2C1_conf.error_handler_callback))(1, BERR);
			CLEAR(I2C1_conf.reg->SR1, 8, 0x1);
		}
		if(READ(I2C1_conf.reg->SR1, 9, 0x1)){
			(*(I2C1_conf.error_handler_callback))(1, ARLO);
			CLEAR(I2C1_conf.reg->SR1, 9, 0x1);
		}
		if(READ(I2C1_conf.reg->SR1, 10, 0x1)){
			(*(I2C1_conf.error_handler_callback))(1, AF);
			CLEAR(I2C1_conf.reg->SR1, 10, 0x1);
		}
		if(READ(I2C1_conf.reg->SR1, 11, 0x1)){
			(*(I2C1_conf.error_handler_callback))(1, OVR);
			CLEAR(I2C1_conf.reg->SR1, 11, 0x1);
		}
		if(READ(I2C1_conf.reg->SR1, 14, 0x1)){
			(*(I2C1_conf.error_handler_callback))(1, TIMEOUT);
			CLEAR(I2C1_conf.reg->SR1, 14, 0x1);
		}
	} else {
		CLEAR(I2C1_conf.reg->SR1, 8, 0xF);
		CLEAR(I2C1_conf.reg->SR1, 14, 0x1);
	}

	CLEAR(I2C1_conf.reg->CR1, 10, 0x1);		// Disable ACK
	CLEAR(I2C1_conf.reg->CR1, 0, 0x1);   	// Disable PE
	SET(I2C1_conf.reg->CR1, 0, 1, 0x1);   	// Reset PE

	return;
}

void I2C2_EV_IRQHandler(void){
	// First detect slave/master mode
	if(READ(I2C2_conf.reg->SR2, 0, 0x1)){  // Master mode
		// Detect event num
		if(READ(I2C2_conf.reg->SR1, 0, 0x1)){  // EV5
			if(I2C2_conf.short_address_bit)  	// 8b mode, send addr
				I2C2_conf.reg->DR = I2C2_conf.status.addr & 0xFF;
			else if(!I2C2_conf.status.Sr){	// 10b mode, send header first
				I2C2_conf.reg->DR = I2C2_conf.status.header;  // Tx header first
				I2C2_conf.status.Sr = true;
			} else
				I2C2_conf.reg->DR = I2C2_conf.status.header + 1;  // Then Rx header in 2nd EV5
		}

		if(READ(I2C2_conf.reg->SR1, 1, 0x1)){  // EV6
			if(I2C2_conf.status.size == 1)	 	// In case of 1byte reception, have to clear ACK bit
				CLEAR(I2C2_conf.reg->CR1, 10, 0x1);  // Have to clear ACK bit before clearing ADDR flag

			if(!READ(I2C2_conf.reg->SR2, 2, 0x1) && I2C2_conf.status.Sr){ 	// Read SR2 to clear ADDR flag and check for Rx mode and 1st EV6
				SET(I2C2_conf.reg->CR1, 8, 1, 0x1);  	// Issue repeated start on Rx mode
				I2C2_conf.status.Sr = false;  // Prevent future Sr
			}
		}

		if(READ(I2C2_conf.reg->SR1, 7, 0x1)){ 	  // TxE = 1 -> transmitting, write byte to DR
			if(I2C2_conf.status.byte_num == I2C2_conf.status.size){ // Last byte to Tx
				while(!READ(I2C2_conf.reg->SR1, 2, 0x1));  // Wait till BTF flag is set (byte transmitted)
				SET(I2C2_conf.reg->CR1, 9, 1, 0x1);		// Program stop condition
				//CLEAR(I2C1_conf.reg->CR1, 10, 0x1);		// Disable ACK

				// Only for LCD2004_App
				GPIO_WriteToPin(&PD13, 0);

				(*(I2C2_conf.master_tx_callback))(2);
				return;
			}
			else
				I2C2_conf.reg->DR = I2C2_conf.status.data[I2C2_conf.status.byte_num++];

		}

		if(READ(I2C2_conf.reg->SR1, 6, 0x1)){  // RxNE = 1 -> receiving, read byte from DR
			I2C2_conf.status.data[I2C2_conf.status.byte_num++] = I2C2_conf.reg->DR & 0xFF;  // Read before issuing NACK

			if(I2C2_conf.status.byte_num == I2C2_conf.status.size){ // 2nd last byte to Rx
				CLEAR(I2C2_conf.reg->CR1, 10, 0x1);   // Disable ACK
				SET(I2C2_conf.reg->CR1, 9, 1, 0x1);   // Program stop condition
				(*(I2C2_conf.master_rx_callback))(2);
				return;
			}
		}

		if(READ(I2C2_conf.reg->SR1, 3, 0x1))  // EV9 - 10b mode only
			I2C2_conf.reg->DR = I2C2_conf.status.addr & 0xFF;

	} else {  // Slave mode
		if(READ(I2C2_conf.reg->SR1, 1, 0x1)){  // EV1, address matched
			I2C2_conf.status.byte_num = 0;  // Init byte num
			I2C2_conf.status.OAR2_matched = READ(I2C2_conf.reg->SR2, 7, 0x1);
			I2C2_conf.status.TX_mode = READ(I2C2_conf.reg->SR2, 2, 0x1);	// TRA = 1 -> slave Tx mode
		}

		if(READ(I2C2_conf.reg->SR1, 7, 0x1))  // EV3 - slave Tx mode
			I2C2_conf.reg->DR = (*(I2C2_conf.slave_tx_callback))(2, I2C2_conf.status.OAR2_matched, I2C2_conf.status.byte_num++) & 0xFF;
		else if(READ(I2C2_conf.reg->SR1, 6, 0x1))  // EV2 - slave Rx mode
			(*(I2C2_conf.slave_rx_callback))(2, I2C2_conf.status.OAR2_matched, I2C2_conf.reg->DR, I2C2_conf.status.byte_num++, false);

		if(READ(I2C2_conf.reg->SR1, 10, 0x1))  // EV3-2 - No ACK received
			CLEAR(I2C2_conf.reg->SR1, 10, 0x1);

		if(READ(I2C2_conf.reg->SR1, 4, 0x1)){   // EV4 - Stop condition detected
			SET(I2C2_conf.reg->CR1, 8, 0x3, 0x3);
			(*(I2C2_conf.slave_rx_callback))(2, I2C2_conf.status.OAR2_matched, 0, I2C2_conf.status.byte_num, true);
		}
	}
}

void I2C2_ER_IRQHandler(void){
	if(I2C2_conf.error_handler_callback != NULL){
		if(READ(I2C2_conf.reg->SR1, 8, 0x1)){
			(*(I2C2_conf.error_handler_callback))(2, BERR);
			CLEAR(I2C2_conf.reg->SR1, 8, 0x1);
		}
		if(READ(I2C2_conf.reg->SR1, 9, 0x1)){
			(*(I2C2_conf.error_handler_callback))(2, ARLO);
			CLEAR(I2C2_conf.reg->SR1, 9, 0x1);
		}
		if(READ(I2C2_conf.reg->SR1, 10, 0x1)){
			(*(I2C2_conf.error_handler_callback))(2, AF);
			CLEAR(I2C2_conf.reg->SR1, 10, 0x1);
		}
		if(READ(I2C2_conf.reg->SR1, 11, 0x1)){
			(*(I2C2_conf.error_handler_callback))(2, OVR);
			CLEAR(I2C2_conf.reg->SR1, 11, 0x1);
		}
		if(READ(I2C2_conf.reg->SR1, 14, 0x1)){
			(*(I2C2_conf.error_handler_callback))(2, TIMEOUT);
			CLEAR(I2C2_conf.reg->SR1, 14, 0x1);
		}
	} else {
		CLEAR(I2C2_conf.reg->SR1, 8, 0xF);
		CLEAR(I2C2_conf.reg->SR1, 14, 0x1);
	}

	CLEAR(I2C2_conf.reg->CR1, 10, 0x1);		// Disable ACK
	CLEAR(I2C2_conf.reg->CR1, 0, 0x1);   	// Disable PE
	SET(I2C2_conf.reg->CR1, 0, 1, 0x1);   	// Reset PE

	return;
}
