/*
 * LCD2004_I2C_driver.h
 *
 *  Created on: Feb 4, 2023
 *      Author: vinht
 */

#ifndef INC_LCD2004_I2C_DRIVER_H_
#define INC_LCD2004_I2C_DRIVER_H_

#include "stm32f4xx.h"
#include "I2C_driver.h"

#define LCD2004_I2C_ADDR 	0x27
#define LCD2004_I2C_Light	0x08
#define LCD2004_I2C_EN		0x04
#define LCD2004_I2C_RW		0x02
#define LCD2004_I2C_RS		0x01
#define LCD2004_I2C_Row0	0x00
#define LCD2004_I2C_Row1	0x40
#define LCD2004_I2C_Row2	0x14
#define LCD2004_I2C_Row3	0x54

typedef struct{
	bool cursor_on;
	bool blinker_on;
	bool cursor_move;
	bool shift_right_mode;
	int I2C_num;
	int I2C_SCL_Alt;
	int I2C_SDA_Alt;

	// Internal use only
	volatile I2C_Config_t * _I2C_conf;
} LCD2004_Config_t;


void LCD2004_Init(LCD2004_Config_t *LCD_conf);
void LCD2004_CMD(LCD2004_Config_t *LCD_conf, unsigned char cmd);
//void LCD2004_Data(LCD2004_Config_t *LCD_conf, unsigned char data);
//unsigned char LCD2004_CharDecode(char data);
void LCD2004_WriteChar(LCD2004_Config_t *LCD_conf, char data);
void LCD2004_WriteString(LCD2004_Config_t *LCD_conf, char *data, unsigned size);
void LCD2004_ClearScreen(LCD2004_Config_t *LCD_conf);
//void LCD2004_NextLine(LCD2004_Config_t *LCD_conf);
void LCD2004_SetCursor(LCD2004_Config_t *LCD_conf, int x, int y);
void LCD2004_I2C_Sender(LCD2004_Config_t *LCD_conf, unsigned char cmd);
//bool LCD2004_isBusy(LCD2004_Config_t *LCD_conf);

#endif /* INC_LCD2004_I2C_DRIVER_H_ */
