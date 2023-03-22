/*
 * LCD1602_driver.h
 *
 *  Created on: Jan 15, 2023
 *      Author: vinht
 */

#ifndef INC_LCD1602_DRIVER_H_
#define INC_LCD1602_DRIVER_H_

#include "stm32f4xx.h"
#include "gpio_driver.h"


typedef struct{
	GPIO_Handle_t *RS;
	GPIO_Handle_t *RW;
	GPIO_Handle_t *E;
	GPIO_Handle_t *D0;
	GPIO_Handle_t *D1;
	GPIO_Handle_t *D2;
	GPIO_Handle_t *D3;
	GPIO_Handle_t *D4;
	GPIO_Handle_t *D5;
	GPIO_Handle_t *D6;
	GPIO_Handle_t *D7;
	bool two_line_mode;
	bool data_width_8b;
	bool font_5x8_mode;
	bool blinker_on;
	bool cursor_move;
	bool shift_right_mode;
} LCD_Config_t;

void LCD_Init(LCD_Config_t *LCD_conf);
void LCD_CMD(LCD_Config_t *LCD_conf, unsigned char cmd);
void LCD_Data(LCD_Config_t *LCD_conf, unsigned char data);
unsigned char LCD_CharDecode(char data);
void LCD_WriteChar(LCD_Config_t *LCD_conf, char data);
void LCD_WriteString(LCD_Config_t *LCD_conf, char *data, unsigned size);
void LCD_ClearScreen(LCD_Config_t *LCD_conf);
void LCD_NextLine(LCD_Config_t *LCD_conf);
void LCD_SetCursor(LCD_Config_t *LCD_conf, int x, int y);
bool LCD_isBusy(LCD_Config_t *LCD_conf);


#endif /* INC_LCD1602_DRIVER_H_ */
