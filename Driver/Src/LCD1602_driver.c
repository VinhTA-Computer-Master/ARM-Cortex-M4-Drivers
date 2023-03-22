/*
 * LCD1602_driver.c
 *
 *  Created on: Jan 15, 2023
 *      Author: vinht
 */


#include "../Inc/LCD1602_driver.h"

void LCD_Data(LCD_Config_t *LCD_conf, unsigned char data){
	if(LCD_conf->data_width_8b){
		if(data & 0x01)
			GPIO_WriteToPin(LCD_conf->D0, 1);
		else
			GPIO_WriteToPin(LCD_conf->D0, 0);

		if(data & 0x02)
			GPIO_WriteToPin(LCD_conf->D1, 1);
		else
			GPIO_WriteToPin(LCD_conf->D1, 0);

		if(data & 0x04)
			GPIO_WriteToPin(LCD_conf->D2, 1);
		else
			GPIO_WriteToPin(LCD_conf->D2, 0);

		if(data & 0x08)
			GPIO_WriteToPin(LCD_conf->D3, 1);
		else
			GPIO_WriteToPin(LCD_conf->D3, 0);

		if(data & 0x10)
			GPIO_WriteToPin(LCD_conf->D4, 1);
		else
			GPIO_WriteToPin(LCD_conf->D4, 0);

		if(data & 0x20)
			GPIO_WriteToPin(LCD_conf->D5, 1);
		else
			GPIO_WriteToPin(LCD_conf->D5, 0);

		if(data & 0x40)
			GPIO_WriteToPin(LCD_conf->D6, 1);
		else
			GPIO_WriteToPin(LCD_conf->D6, 0);

		if(data & 0x80)
			GPIO_WriteToPin(LCD_conf->D7, 1);
		else
			GPIO_WriteToPin(LCD_conf->D7, 0);

	} else {  // 4 bit data mode
		if(data & 0x01)
			GPIO_WriteToPin(LCD_conf->D4, 1);
		else
			GPIO_WriteToPin(LCD_conf->D4, 0);

		if(data & 0x02)
			GPIO_WriteToPin(LCD_conf->D5, 1);
		else
			GPIO_WriteToPin(LCD_conf->D5, 0);

		if(data & 0x04)
			GPIO_WriteToPin(LCD_conf->D6, 1);
		else
			GPIO_WriteToPin(LCD_conf->D6, 0);

		if(data & 0x08)
			GPIO_WriteToPin(LCD_conf->D7, 1);
		else
			GPIO_WriteToPin(LCD_conf->D7, 0);
	}
}

void LCD_CMD(LCD_Config_t *LCD_conf, unsigned char cmd){
	GPIO_WriteToPin(LCD_conf->E, 0);
	GPIO_WriteToPin(LCD_conf->RS, 0);
	GPIO_WriteToPin(LCD_conf->RW, 0);

	LCD_Data(LCD_conf, cmd);

	GPIO_WriteToPin(LCD_conf->E, 1);
	msec_delay(1);
	GPIO_WriteToPin(LCD_conf->E, 0);
	msec_delay(1);
}

void LCD_Init(LCD_Config_t *LCD_conf){
	// GPIOs have to be init before, moder set by this function
	GPIO_SetModer(LCD_conf->RS, MODER_OUTPUT);
	GPIO_SetModer(LCD_conf->RW, MODER_OUTPUT);
	GPIO_SetModer(LCD_conf->E, MODER_OUTPUT);
	if(LCD_conf->data_width_8b){
		GPIO_SetModer(LCD_conf->D0, MODER_OUTPUT);
		GPIO_SetModer(LCD_conf->D1, MODER_OUTPUT);
		GPIO_SetModer(LCD_conf->D2, MODER_OUTPUT);
		GPIO_SetModer(LCD_conf->D3, MODER_OUTPUT);
	}
	GPIO_SetModer(LCD_conf->D4, MODER_OUTPUT);
	GPIO_SetModer(LCD_conf->D5, MODER_OUTPUT);
	GPIO_SetModer(LCD_conf->D6, MODER_OUTPUT);
	GPIO_SetModer(LCD_conf->D7, MODER_OUTPUT);

	GPIO_WriteToPin(LCD_conf->RS, 0);
	GPIO_WriteToPin(LCD_conf->RW, 0);
	GPIO_WriteToPin(LCD_conf->E, 0);
	LCD_Data(LCD_conf, 0x00);

	msec_delay(150);

	for(int i=0; i<3; i++){
		if(LCD_conf->data_width_8b)
			LCD_CMD(LCD_conf, 0x30);
		else
			LCD_CMD(LCD_conf, 0x03);
		msec_delay(5);
	}

	// Function set
	if(LCD_conf->data_width_8b){
		unsigned char cmd = 0x38;
		if(!LCD_conf->two_line_mode)
			CLEAR(cmd, 3, 0x1);
		if(!LCD_conf->font_5x8_mode)
			SET(cmd, 2, 0x1, 0x1);
		LCD_CMD(LCD_conf, cmd);
	} else {
		LCD_CMD(LCD_conf, 0x02);
		LCD_CMD(LCD_conf, 0x02);
		unsigned char cmd = 0x08;
		if(!LCD_conf->two_line_mode)
			CLEAR(cmd, 3, 0x1);
		if(!LCD_conf->font_5x8_mode)
			SET(cmd, 2, 0x1, 0x1);
		LCD_CMD(LCD_conf, cmd);
	}

	// Display setting
	if(LCD_conf->data_width_8b){
		unsigned char cmd = 0x0F;
		if(!LCD_conf->blinker_on)
			CLEAR(cmd, 0, 0x1);
		LCD_CMD(LCD_conf, cmd);
	} else {
		LCD_CMD(LCD_conf, 0x00);
		unsigned char cmd = 0x0F;
		if(!LCD_conf->blinker_on)
			CLEAR(cmd, 0, 0x1);
		LCD_CMD(LCD_conf, cmd);
	}

	// Cursor setting
	if(LCD_conf->data_width_8b){
		unsigned char cmd = 0x14;
		if(!LCD_conf->cursor_move)
			SET(cmd, 3, 1, 0x1);
		if(!LCD_conf->shift_right_mode)
			CLEAR(cmd, 2, 0x1);
		LCD_CMD(LCD_conf, cmd);
	} else {
		LCD_CMD(LCD_conf, 0x01);
		unsigned char cmd = 0x04;
		if(!LCD_conf->cursor_move)
			SET(cmd, 3, 1, 0x1);
		if(!LCD_conf->shift_right_mode)
			CLEAR(cmd, 2, 0x1);
		LCD_CMD(LCD_conf, cmd);
	}

	// Entry mode
	if(!LCD_conf->data_width_8b)
		LCD_CMD(LCD_conf, 0x00);
	LCD_CMD(LCD_conf, 0x06);

	// Clear display
	LCD_ClearScreen(LCD_conf);
}

// Extensive list - more to be added later
unsigned char LCD_CharDecode(char data){
	switch(data){
	case 'A': return 0x41;
	case 'B': return 0x42;
	case 'C': return 0x43;
	case 'D': return 0x44;
	case 'E': return 0x45;
	case 'F': return 0x46;
	case 'G': return 0x47;
	case 'H': return 0x48;
	case 'I': return 0x49;
	case 'J': return 0x4A;
	case 'K': return 0x4B;
	case 'L': return 0x4C;
	case 'M': return 0x4D;
	case 'N': return 0x4E;
	case 'O': return 0x4F;
	case 'P': return 0x50;
	case 'Q': return 0x51;
	case 'R': return 0x52;
	case 'S': return 0x53;
	case 'T': return 0x54;
	case 'U': return 0x55;
	case 'V': return 0x56;
	case 'W': return 0x57;
	case 'X': return 0x58;
	case 'Y': return 0x59;
	case 'Z': return 0x5A;
	case 'a': return 0x61;
	case 'b': return 0x62;
	case 'c': return 0x63;
	case 'd': return 0x64;
	case 'e': return 0x65;
	case 'f': return 0x66;
	case 'g': return 0x67;
	case 'h': return 0x68;
	case 'i': return 0x69;
	case 'j': return 0x6A;
	case 'k': return 0x6B;
	case 'l': return 0x6C;
	case 'm': return 0x6D;
	case 'n': return 0x6E;
	case 'o': return 0x6F;
	case 'p': return 0x70;
	case 'q': return 0x71;
	case 'r': return 0x72;
	case 's': return 0x73;
	case 't': return 0x74;
	case 'u': return 0x75;
	case 'v': return 0x76;
	case 'w': return 0x77;
	case 'x': return 0x78;
	case 'y': return 0x79;
	case 'z': return 0x7A;
	case '0': return 0x30;
	case '1': return 0x31;
	case '2': return 0x32;
	case '3': return 0x33;
	case '4': return 0x34;
	case '5': return 0x35;
	case '6': return 0x36;
	case '7': return 0x37;
	case '8': return 0x38;
	case '9': return 0x39;
	case ':': return 0x3A;
	case ';': return 0x3B;
	case '<': return 0x3C;
	case '=': return 0x3D;
	case '>': return 0x3E;
	case '?': return 0x3F;
	case ' ': return 0x20;
	case '!': return 0x21;
	case '"': return 0x22;
	case '#': return 0x23;
	case '$': return 0x24;
	case '%': return 0x25;
	case '&': return 0x26;
	case '\'': return 0x27;
	case '(': return 0x28;
	case ')': return 0x29;
	case '*': return 0x2A;
	case '+': return 0x2B;
	case ',': return 0x2C;
	case '-': return 0x2D;
	case '.': return 0x2E;
	case '/': return 0x2F;
	case '\n': return 0xFF;
	}
	return 0x20;
}

void LCD_WriteChar(LCD_Config_t *LCD_conf, char data){
	GPIO_WriteToPin(LCD_conf->RS, 1);
	GPIO_WriteToPin(LCD_conf->RW, 0);
	GPIO_WriteToPin(LCD_conf->E, 0);

	unsigned char wdata = LCD_CharDecode(data);
	if(LCD_conf->data_width_8b){
		LCD_Data(LCD_conf, wdata);
		GPIO_WriteToPin(LCD_conf->E, 1);
		msec_delay(1);
		//while(LCD_isBusy(LCD_conf));
		GPIO_WriteToPin(LCD_conf->E, 0);
		msec_delay(1);
	} else {
		LCD_Data(LCD_conf, (wdata >> 4) & 0x0F);
		GPIO_WriteToPin(LCD_conf->E, 1);
		msec_delay(1);
		//while(LCD_isBusy(LCD_conf));
		GPIO_WriteToPin(LCD_conf->E, 0);
		msec_delay(1);

		LCD_Data(LCD_conf, (wdata & 0x0F));
		GPIO_WriteToPin(LCD_conf->E, 1);
		msec_delay(1);
		//while(LCD_isBusy(LCD_conf));
		GPIO_WriteToPin(LCD_conf->E, 0);
		msec_delay(1);
	}
}

void LCD_WriteString(LCD_Config_t *LCD_conf, char *data, unsigned size){
	unsigned line_count = 0;
	unsigned line = 0;
	for(int i = 0; i < size; i++){
		if(data[i] != '\n' && data[i] != 0){
			LCD_WriteChar(LCD_conf, data[i]);
			line_count++;
		} else if(data[i] == 0)
			return;
		else {
			LCD_NextLine(LCD_conf);
			line_count = 0;
			line++;
		}

		if((line_count > 16 && data[i+1] != '\n') || (data[i+1] == '\n' && line == 1)){
			LCD_ClearScreen(LCD_conf);
			LCD_WriteChar(LCD_conf, 'S');
			LCD_WriteChar(LCD_conf, 'c');
			LCD_WriteChar(LCD_conf, 'r');
			LCD_WriteChar(LCD_conf, 'e');
			LCD_WriteChar(LCD_conf, 'e');
			LCD_WriteChar(LCD_conf, 'n');
			LCD_NextLine(LCD_conf);
			LCD_WriteChar(LCD_conf, 'O');
			LCD_WriteChar(LCD_conf, 'v');
			LCD_WriteChar(LCD_conf, 'e');
			LCD_WriteChar(LCD_conf, 'r');
			LCD_WriteChar(LCD_conf, 'f');
			LCD_WriteChar(LCD_conf, 'l');
			LCD_WriteChar(LCD_conf, 'o');
			LCD_WriteChar(LCD_conf, 'w');
			LCD_WriteChar(LCD_conf, '!');
			LCD_WriteChar(LCD_conf, '!');
			LCD_WriteChar(LCD_conf, '!');
			return;
		}
	}
}

void LCD_ClearScreen(LCD_Config_t *LCD_conf){
	/*if(!LCD_conf->data_width_8b)
		LCD_CMD(LCD_conf, 0x00);
	LCD_CMD(LCD_conf, 0x01);*/
	LCD_SetCursor(LCD_conf, 0, 0);
	for(int i=0; i<16; i++)
		LCD_WriteChar(LCD_conf, 0);

	LCD_SetCursor(LCD_conf, 0, 1);
	for(int i=0; i<16; i++)
		LCD_WriteChar(LCD_conf, 0);

	LCD_SetCursor(LCD_conf, 0, 0);
}

void LCD_NextLine(LCD_Config_t *LCD_conf){
	if(!LCD_conf->data_width_8b){
		LCD_CMD(LCD_conf, 0x0C);
		LCD_CMD(LCD_conf, 0x00);
	} else
		LCD_CMD(LCD_conf, 0xC0);
}

void LCD_SetCursor(LCD_Config_t *LCD_conf, int x, int y){
	unsigned addr = 0x00;
	if(y == 1)
		addr = 0x40;
	addr += (x & 0xF);
	unsigned cmd = 0x80 + (addr & 0x7F);

	if(!LCD_conf->data_width_8b){
		LCD_CMD(LCD_conf, (cmd>>4)&0x0F);
		LCD_CMD(LCD_conf, cmd & 0x0F);
	} else
		LCD_CMD(LCD_conf, cmd);
}

bool LCD_isBusy(LCD_Config_t *LCD_conf){
	GPIO_WriteToPin(LCD_conf->RS, 0);
	GPIO_WriteToPin(LCD_conf->RW, 1);
	//GPIO_WriteToPin(LCD_conf->E, 1);
	bool ret = GPIO_ReadFromPin(LCD_conf->D7);
	//GPIO_WriteToPin(LCD_conf->E, 0);
	//GPIO_WriteToPin(LCD_conf->RW, 0);

	return ret;
}
