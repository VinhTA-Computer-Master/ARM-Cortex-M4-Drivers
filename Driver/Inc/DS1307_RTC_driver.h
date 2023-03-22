/*
 * DS1307_RTC_driver.h
 *
 *  Created on: Jan 24, 2023
 *      Author: vinht
 */

#ifndef INC_DS1307_RTC_DRIVER_H_
#define INC_DS1307_RTC_DRIVER_H_

#include "stm32f4xx.h"
#include "I2C_driver.h"

#define DS1307_RTC_ADDR 	0x68

typedef struct{
	unsigned I2C_num;
	unsigned I2C_SCL_Alt_Pin;
	unsigned I2C_SDA_Alt_Pin;
	volatile bool busy;
} DS1307_Handle_t;

// Only 1 RTC module is allowed -> config is included here and used by this library
volatile DS1307_Handle_t RTC_Config;

void RTC_Init(); 	// Mainly init I2C instance
void RTC_DeInit();	// Disable I2C
void RTC_set_second(unsigned sec);
void RTC_set_minute(unsigned min);
void RTC_set_hour(unsigned hour);  	 // 24 hour format only!
void RTC_set_weekday(unsigned day);  // 1 = Sun, 2 = Mon
void RTC_set_date(unsigned date);
void RTC_set_month(unsigned mon);
void RTC_set_year(unsigned yr);
void RTC_set_time(unsigned sec, unsigned min, unsigned hour, unsigned day, unsigned date, unsigned mon, unsigned yr);
void RTC_set_clock_output(bool en, unsigned freq);  // 0 - 1Hz, 1 - 4096Hz, 2 - 8192Hz, 3 - 32768Hz
void RTC_disable_clock();  	// Keep clock from running after shutdown, also resets seconds

unsigned RTC_get_second();
unsigned RTC_get_minute();
unsigned RTC_get_hour();
unsigned RTC_get_weekday();
unsigned RTC_get_date();
unsigned RTC_get_month();
unsigned RTC_get_year();

void tx_callback(int);
void rx_callback(int);
void error_callback(int, I2C_Error);

#endif /* INC_DS1307_RTC_DRIVER_H_ */
