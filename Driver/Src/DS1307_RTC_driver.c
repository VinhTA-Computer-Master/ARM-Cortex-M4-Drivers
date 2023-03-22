/*
 * DS1307_RTC_driver.c
 *
 *  Created on: Jan 24, 2023
 *      Author: vinht
 */


#include "../Inc/DS1307_RTC_driver.h"

volatile DS1307_Handle_t RTC_Config = {1, 0, 0, false};
volatile I2C_Config_t* I2C_Config;
unsigned char data[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Data holder for transmit/receive

void tx_callback(int dummy){
	RTC_Config.busy = false;
}

void rx_callback(int dummy){
	RTC_Config.busy = false;
}

void error_callback(int dummy, I2C_Error err_code){
	RTC_Config.busy = false;
}

void RTC_Init(){
	if(RTC_Config.I2C_num == 1)
		I2C_Config = &I2C1_conf;
	else if(RTC_Config.I2C_num == 2)
		I2C_Config = &I2C2_conf;
	else
		I2C_Config = &I2C3_conf;

	I2C_Config->SCL_alt_pin = RTC_Config.I2C_SCL_Alt_Pin;
	I2C_Config->SDA_alt_pin = RTC_Config.I2C_SDA_Alt_Pin;
	I2C_Config->master_tx_callback = &tx_callback;
	I2C_Config->master_rx_callback = &rx_callback;
	I2C_Config->error_handler_callback = &error_callback;

	I2C_Init(I2C_Config);
}

void RTC_DeInit(){
	I2C_Reset(I2C_Config);
}

void RTC_disable_clock(){
	data[0] = 0x00;
	data[1] = 0x80;

	RTC_Config.busy = true;
	I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2);
	while(RTC_Config.busy);
}

void RTC_set_second(unsigned sec){
	data[0] = 0x00;  // Reg addr for sec
	data[1] = 0x00;
	data[1] = ((sec/10) & 0x07) << 4;
	data[1] |= ((sec%10) & 0x0F);

	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2))
		while(1);

	while(RTC_Config.busy);
}

void RTC_set_minute(unsigned min){
	data[0] = 0x01;
	data[1] = 0x00;
	data[1] = ((min/10) & 0x07) << 4;
	data[1] |= ((min%10) & 0x0F);

	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2))
		while(1);
	while(RTC_Config.busy);
}

void RTC_set_hour(unsigned hour){
	data[0] = 0x02;
	data[1] = ((hour/10) & 0x03) << 4;
	data[1] |= ((hour%10) & 0x0F);
	data[1] &= ~(1 << 6);  // Set 24h mode

	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2))
		while(1);
	while(RTC_Config.busy);
	//while(READ(I2C_Config->reg->SR2, 1, 0x1));
}

void RTC_set_weekday(unsigned day){
	data[0] = 0x03;
	data[1] = day & 0x07;

	RTC_Config.busy = true;
	I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2);
	while(RTC_Config.busy);
}

void RTC_set_date(unsigned date){
	data[0] = 0x04;
	data[1] = ((date/10) & 0x03) << 4;
	data[1] |= ((date%10) & 0x0F);

	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2))
		while(1);
	while(RTC_Config.busy);
}

void RTC_set_month(unsigned mon){
	data[0] = 0x05;
	data[1] = ((mon/10) & 0x01) << 4;
	data[1] |= ((mon%10) & 0x0F);

	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2))
		while(1);
	while(RTC_Config.busy);
}

void RTC_set_year(unsigned yr){
	data[0] = 0x06;
	data[1] = ((yr/10) & 0x0F) << 4;
	data[1] |= ((yr%10) & 0x0F);

	RTC_Config.busy = true;
	I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2);
	while(RTC_Config.busy);
}

void RTC_set_time(unsigned sec, unsigned min, unsigned hour, unsigned day, unsigned date, unsigned mon, unsigned yr){
	data[0] = 0x00;

	// Sec
	data[1] = 0x00;
	data[1] = ((sec/10) & 0x07) << 4;
	data[1] |= ((sec%10) & 0x0F);

	// Min
	data[2] = 0x00;
	data[2] = ((min/10) & 0x07) << 4;
	data[2] |= ((min%10) & 0x0F);

	// Hour
	data[3] = ((hour/10) & 0x03) << 4;
	data[3] |= ((hour%10) & 0x0F);
	data[3] |= 1 << 6;

	// Day
	data[4] = day & 0x07;

	// Date
	data[5] = ((date/10) & 0x03) << 4;
	data[5] |= ((date%10) & 0x0F);

	// Month
	data[6] = ((mon/10) & 0x01) << 4;
	data[6] |= ((mon%10) & 0x0F);

	// Year
	data[7] = ((yr/10) & 0x0F) << 4;
	data[7] |= ((yr%10) & 0x0F);

	RTC_Config.busy = true;
	I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 8);
	while(RTC_Config.busy);
}

void RTC_set_clock_output(bool en, unsigned freq){
	data[0] = 0x07;
	data[1] = en << 4;
	data[1] |= freq & 0x03;

	RTC_Config.busy = true;
	I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 2);
	while(RTC_Config.busy);
}

unsigned RTC_get_second(){
	data[0] = 0x00;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	//msec_delay(100);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	unsigned ret = 0;
	ret = ((data[1] & 0x70) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}

unsigned RTC_get_minute(){
	data[0] = 0x01;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	//msec_delay(100);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	unsigned ret = 0;
	ret = ((data[1] & 0x70) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}

unsigned RTC_get_hour(){
	data[0] = 0x02;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	//msec_delay(100);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	unsigned ret = 0;
	ret = ((data[1] & 0x30) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}

unsigned RTC_get_weekday(){
	data[0] = 0x03;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);  // Wait for data to arrive in callback

	unsigned ret = (data[1] & 0x03);
	return ret;
}

unsigned RTC_get_date(){
	data[0] = 0x04;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	//msec_delay(100);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	unsigned ret = 0;
	ret = ((data[1] & 0x30) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}

unsigned RTC_get_month(){
	data[0] = 0x05;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	//msec_delay(100);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	unsigned ret = 0;
	ret = ((data[1] & 0x10) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}

unsigned RTC_get_year(){
	data[0] = 0x06;

	// Set reg pointer first
	RTC_Config.busy = true;
	if(!I2C_SendData(I2C_Config, DS1307_RTC_ADDR, &data, 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);

	// Read from that pointer
	RTC_Config.busy = true;
	if(!I2C_RequestData(I2C_Config, DS1307_RTC_ADDR, &(data[1]), 1)){
		RTC_Config.busy = false;
		return 0;
	}
	while(RTC_Config.busy);  // Wait for data to arrive in callback

	unsigned ret = 0;
	ret = ((data[1] & 0xF0) >> 4)*10;
	ret += (data[1] & 0x0F);
	return ret;
}
