/*
 * I2C_driver.h
 *
 *  Created on: Jan 16, 2023
 *      Author: vinht
 */

#ifndef INC_I2C_DRIVER_H_
#define INC_I2C_DRIVER_H_

#include "stm32f4xx.h"
#include "gpio_driver.h"

/*
 * I2C1: PB6/8 (SCL) & PB7/9 (SDA)
 * I2C2: PB10/PF1/PH4 (SCL) & PB11/PF0/PH5 (SDA)
 * I2C3: PH7/PA8 (SCL) & PH8/PC9 (SDA)
 * No support for DMA, SMB, PEC, Gen Call
 * Default setting: 16MHz bus freq, clock stretch, event/buffer int, 100kHz/400kHz SCL, 50% duty, no digital filter
 *   Max Trise (1000ns/250ns for SM/FM)
 */

typedef enum{
	BERR = 1,
	ARLO,
	AF,
	OVR,
	TIMEOUT
} I2C_Error;

typedef struct{
	int byte_num;
	int size;
	// For slave mode
	bool OAR2_matched;
	bool TX_mode;
	// For master mode
	bool Sr;  // For repeated start in 10b Rx mode
	unsigned char addr;  // always 8b
	unsigned header;  // For 10b mode
	unsigned char *data;
} I2C_status;

typedef struct{
	// I2C settings - pre-configured in .c file
	bool error_handler;		// Default true
	bool short_address_bit; // 7b address mode - default true
	bool dual_address;		// Default false
	bool fast_mode;   		// 400kHz mode - default false - not recommended
	unsigned char SCL_alt_pin;	// Default 0
	unsigned char SDA_alt_pin;	// Default 0
	unsigned char I2C_num;
	unsigned bus_freq;    	// In MHz (from 2-50MHz only)

	// For slave mode only
	unsigned char ADDR2;  	// For dual address slave mode only
	unsigned ADDR;			// For slave mode only - should be <= 0x7F in short addr mode (7b)

	// Application callback functions - to be assigned by driver
	void (*master_tx_callback)(int I2C_num);	// Callback when data transmit is done
	void (*master_rx_callback)(int I2C_num);  	// Callback when request data is done
	unsigned char (*slave_tx_callback)(int I2C_num, bool OAR2, unsigned byte_num); 	// Slave tx mode, callback for data to be sent
	void (*slave_rx_callback)(int I2C_num, bool OAR2, unsigned char data, unsigned byte_num, bool stop);	// Slave rx mode, callback for handling received data
	void (*error_handler_callback)(int I2C_num, I2C_Error err);  // Optional

	// For internal use only
	bool isInit;
	volatile I2C_status status;
	I2Cx_reg * reg;
} I2C_Config_t;


// Config & Handle structs for I2C 1/2/3 modules
volatile I2C_Config_t I2C1_conf, I2C2_conf, I2C3_conf;

void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void I2C3_EV_IRQHandler(void);
void I2C3_ER_IRQHandler(void);

void I2C_Init(volatile I2C_Config_t *conf);
void I2C_Reset(volatile I2C_Config_t *conf);
bool I2C_SendData(volatile I2C_Config_t *conf, unsigned char addr, unsigned char *data, unsigned num_bytes);  // Non-blocking -> data only sent in interrupts, return false on errors
bool I2C_RequestData(volatile I2C_Config_t *conf, unsigned char addr, unsigned char *data, unsigned num_bytes);  // Non-blocking -> data only arrives in interrupts
bool I2C_isBusy(volatile I2C_Config_t *conf);

#endif /* INC_I2C_DRIVER_H_ */
