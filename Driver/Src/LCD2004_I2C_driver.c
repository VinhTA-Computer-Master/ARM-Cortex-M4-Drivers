/*
 * LCD2004_I2C_driver.c
 *
 *  Created on: Feb 4, 2023
 *      Author: vinht
 */


#include "../Inc/LCD2004_I2C_driver.h"
#include "../Inc/LCD1602_driver.h"

void LCD2004_tx_callback(int dummy){

}

void LCD2004_rx_callback(int dummy){

}

void LCD2004_error_callback(int dummy, I2C_Error err){
	while(1);
}


void LCD2004_Init(LCD2004_Config_t *LCD_conf){
	if(LCD_conf->I2C_num == 1){
		I2C1_conf.master_tx_callback = &LCD2004_tx_callback;
		I2C1_conf.master_rx_callback = &LCD2004_rx_callback;
		I2C1_conf.error_handler_callback = &LCD2004_error_callback;
		I2C1_conf.SCL_alt_pin = LCD_conf->I2C_SCL_Alt;
		I2C1_conf.SDA_alt_pin = LCD_conf->I2C_SDA_Alt;
		//I2C1_conf.bus_freq = abp1_frequency;
		LCD_conf->_I2C_conf = &I2C1_conf;
		I2C_Init(&I2C1_conf);
	} else if(LCD_conf->I2C_num == 2){
		I2C2_conf.master_tx_callback = &LCD2004_tx_callback;
		I2C2_conf.master_rx_callback = &LCD2004_rx_callback;
		I2C2_conf.error_handler_callback = &LCD2004_error_callback;
		LCD_conf->_I2C_conf = &I2C2_conf;
		I2C2_conf.SCL_alt_pin = LCD_conf->I2C_SCL_Alt;
		I2C2_conf.SDA_alt_pin = LCD_conf->I2C_SDA_Alt;
		//I2C2_conf.bus_freq = abp1_frequency;
		I2C_Init(&I2C2_conf);
	} else {
		I2C3_conf.master_tx_callback = &LCD2004_tx_callback;
		I2C3_conf.master_rx_callback = &LCD2004_rx_callback;
		I2C3_conf.error_handler_callback = &LCD2004_error_callback;
		LCD_conf->_I2C_conf = &I2C3_conf;
		I2C3_conf.SCL_alt_pin = LCD_conf->I2C_SCL_Alt;
		I2C3_conf.SDA_alt_pin = LCD_conf->I2C_SDA_Alt;
		//I2C3_conf.bus_freq = abp1_frequency;
		I2C_Init(&I2C3_conf);
	}

	msec_delay(150);

	for(int i=0; i<3; i++){
		LCD2004_CMD(LCD_conf, 0x30);	// On abstract level (above CMD), 8b interface is used
		msec_delay(5);					// CMD function will take care of 8b to 4b interface
	}

	LCD2004_CMD(LCD_conf, 0x20);	// 4b interface
	msec_delay(100);
	LCD2004_CMD(LCD_conf, 0x20);	// 4 line mode, 0x28 -> 0x20

	unsigned char cmd = 0x0F;		// Display always on, 0x0F->0x0C
	if(!LCD_conf->blinker_on)
		CLEAR(cmd, 0, 0x1);
	if(!LCD_conf->cursor_on)
		CLEAR(cmd, 1, 0x1);
	LCD2004_CMD(LCD_conf, cmd);

	msec_delay(100);

	/*cmd = 0x14;		// Default cursor move & shift right
	if(!LCD_conf->cursor_move)
		SET(cmd, 3, 1, 0x1);
	if(!LCD_conf->shift_right_mode)
		CLEAR(cmd, 2, 0x1);
	LCD2004_CMD(LCD_conf, cmd);
	msec_delay(1000);*/

	LCD2004_ClearScreen(LCD_conf);

	// Entry mode
	LCD2004_CMD(LCD_conf, 0x06);

	// Clear display
	LCD2004_ClearScreen(LCD_conf);

	LCD2004_I2C_Sender(LCD_conf, 0x0);
	LCD2004_SetCursor(LCD_conf, 0, 0);

	msec_delay(100);
	LCD2004_ClearScreen(LCD_conf);
}

// This function wraps I2C communication and takes care of EN pulse and backlight bits, but not RS/RW bits
void LCD2004_I2C_Sender(LCD2004_Config_t *LCD_conf, unsigned char cmd){
	//I2C_SendData(LCD_conf->_I2C_conf, LCD2004_I2C_ADDR, 0x00, 1);
	//while(READ(LCD_conf->_I2C_conf->reg->SR2, 1, 0x1));

	unsigned char data = cmd | LCD2004_I2C_Light;

	I2C_SendData(LCD_conf->_I2C_conf, LCD2004_I2C_ADDR, &data, 1);
	while(I2C_isBusy(LCD_conf->_I2C_conf));
	msec_delay(10);

	data |= LCD2004_I2C_EN;
	I2C_SendData(LCD_conf->_I2C_conf, LCD2004_I2C_ADDR, &data, 1);
	while(I2C_isBusy(LCD_conf->_I2C_conf));
	msec_delay(10);

	data &= ~LCD2004_I2C_EN;
	I2C_SendData(LCD_conf->_I2C_conf, LCD2004_I2C_ADDR, &data, 1);
	while(I2C_isBusy(LCD_conf->_I2C_conf));
	msec_delay(10);
}

void LCD2004_CMD(LCD2004_Config_t *LCD_conf, unsigned char cmd){
	unsigned char cmd_high = (cmd & 0xF0);
	unsigned char cmd_low = (cmd & 0x0F) << 4;

	LCD2004_I2C_Sender(LCD_conf, cmd_high);
	LCD2004_I2C_Sender(LCD_conf, cmd_low);
}

void LCD2004_ClearScreen(LCD2004_Config_t *LCD_conf){
	LCD2004_CMD(LCD_conf, 0x01);
	msec_delay(300);
}

void LCD2004_SetCursor(LCD2004_Config_t *LCD_conf, int row, int col){
	unsigned char addr = 0x00;

	if(row == 0)
		addr = LCD2004_I2C_Row0;
	else if(row == 1)
		addr = LCD2004_I2C_Row1;
	else if(row == 2)
		addr = LCD2004_I2C_Row2;
	else if(row == 3)
		addr = LCD2004_I2C_Row3;

	addr += (col >= 20)? 19:col;

	LCD2004_CMD(LCD_conf, 0x80 | (addr & 0x7F));
}

void LCD2004_WriteChar(LCD2004_Config_t *LCD_conf, char data){
	unsigned char wdata = LCD_CharDecode(data);		// Taken from LCD1602 library API
	unsigned char wdata_high = (wdata & 0xF0) | LCD2004_I2C_RS;
	unsigned char wdata_low = ((wdata << 4) & 0xF0) | LCD2004_I2C_RS;

	LCD2004_I2C_Sender(LCD_conf, wdata_high);
	LCD2004_I2C_Sender(LCD_conf, wdata_low);
}

// As there are 4 lines and there's no way to tell current line,
// this library does not implement NextLine and '\n' is interpreted as ' '
// overflow is also not insensitive to this API
// Cursor should also be set before calling this function
void LCD2004_WriteString(LCD2004_Config_t *LCD_conf, char *data, unsigned size){
	for(unsigned i = 0; i < size; i++){
		//if(data[i] == '\n')
			//LCD2004_WriteChar(LCD_conf, ' ');
		if(data[i] == 0)
			return;
		else
			LCD2004_WriteChar(LCD_conf, data[i]);
	}
}
